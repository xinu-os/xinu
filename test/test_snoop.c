#include <stddef.h>
#include <device.h>
#include <ipv4.h>
#include <limits.h>
#include <network.h>
#include <snoop.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <testsuite.h>

#if NETHER

#ifndef ELOOP
#define ELOOP (-1)
#endif

#ifndef NNETIF
#define NNETIF (-1)
#endif

extern int _binary_data_testsnoop_pcap_start;

static uint filterTest(struct snoop *cap, struct packet *pktA)
{
    struct pcap_file_header pcap;
    struct pcap_pkthdr phdr;
    uchar *data;
    uint nmatch = 0;
    int i;

    data = (uchar *)(&_binary_data_testsnoop_pcap_start);
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);
    for (i = 0; i < 17; i++)
    {
        memcpy(&phdr, data, sizeof(phdr));
        data += sizeof(phdr);
        if (PCAP_MAGIC != pcap.magic)
        {
            phdr.caplen = endswap(phdr.caplen);
        }
        pktA->len = phdr.caplen;
        pktA->curr = pktA->data;
        memcpy(pktA->data, data, phdr.caplen);
        if (TRUE == snoopFilter(cap, pktA))
        {
            nmatch++;
        }
        data += phdr.caplen;
    }

    return nmatch;
}

#endif /* NETHER */


/**
 * Test for snoop.
 */
thread test_snoop(bool verbose)
{
#if NETHER
    bool passed = TRUE;
    struct snoop cap;
    struct netaddr dst;
    struct netaddr src;
    struct netaddr mask;
    struct netif *netptr;
    struct pcap_file_header pcap;
    struct pcap_pkthdr phdr;
    struct packet *pktA;
    struct packet *pktB;
    uchar *data;
    int i;

    src.len = IPv4_ADDR_LEN;
    src.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
    dst.type = NETADDR_IPv4;
    mask.len = IPv4_ADDR_LEN;
    mask.type = NETADDR_IPv4;

    src.addr[0] = 192;
    src.addr[1] = 168;
    src.addr[2] = 1;
    src.addr[3] = 6;

    dst.addr[0] = 192;
    dst.addr[1] = 168;
    dst.addr[2] = 1;
    dst.addr[3] = 1;

    /* Initialization */
    testPrint(verbose, "Test case initialization");
    pktA = netGetbuf();
    failif((SYSERR == (int)pktA), "Failed get buffer");
    if (!passed)
    {
        testFail(TRUE, "");
        return OK;
    }

    /* Test filter */

    /* Filter type */
    testPrint(verbose, "Filter type");
    bzero(&cap, sizeof(struct snoop));
    cap.caplen = USHRT_MAX;
    cap.type = SNOOP_FILTER_ARP;
    failif((7 != filterTest(&cap, pktA)), "");

    /* Test open */
    testPrint(verbose, "Open capture (bad params)");
    bzero(&cap, sizeof(struct snoop));
    failif((SYSERR != snoopOpen(NULL, NULL)), "");

    testPrint(verbose, "Open capture (bad device)");
    failif((SYSERR != snoopOpen(&cap, "not-a-dev")), "");

    testPrint(verbose, "Open capture all (no netif)");
    failif((SYSERR != snoopOpen(&cap, "ALL")), "");

    src.addr[0] = 192;
    src.addr[1] = 168;
    src.addr[2] = 1;
    src.addr[3] = 6;

    dst.addr[0] = 192;
    dst.addr[1] = 168;
    dst.addr[2] = 1;
    dst.addr[3] = 1;

    mask.addr[0] = 192;
    mask.addr[1] = 168;
    mask.addr[2] = 1;
    mask.addr[3] = 1;

    open(ELOOP);
    netUp(ELOOP, &src, &dst, &mask);

    testPrint(verbose, "Open capture all");
    if (SYSERR == snoopOpen(&cap, "ALL"))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        for (i = 0; i < NNETIF; i++)
        {
            if ((NET_ALLOC == netiftab[i].state)
                && (NULL == netiftab[i].capture))
            {
                break;
            }
        }
        failif((i < NNETIF), "Not attached to all");
    }

    testPrint(verbose, "Close capture (bad params)");
    failif((SYSERR != snoopClose(NULL)), "");

    testPrint(verbose, "Close capture");
    if (SYSERR == snoopClose(&cap))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        for (i = 0; i < NNETIF; i++)
        {
            if (&cap == netiftab[i].capture)
            {
                break;
            }
        }
        failif((i < NNETIF), "Not removed from all");
    }
    testPrint(verbose, "Open capture on ELOOP");
    bzero(&cap, sizeof(struct snoop));
    netptr = NULL;
    if (SYSERR == snoopOpen(&cap, "ELOOP"))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        for (i = 0; i < NNETIF; i++)
        {
            if (ELOOP == netiftab[i].dev)
            {
                netptr = &netiftab[i];
                break;
            }
        }
        failif(((NULL == netptr) || (&cap != netptr->capture)),
               "Not attached to ELOOP");
    }

    testPrint(verbose, "Capture (bad params)");
    failif((SYSERR != snoopCapture(NULL, NULL)), "");

    /* Reset data stream to beginning of PCAP file */
    data = (uchar *)(&_binary_data_testsnoop_pcap_start);
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }

    testPrint(verbose, "Capture no match");
    memcpy(pktA->data, data, phdr.caplen);
    pktA->len = phdr.caplen;
    pktA->nif = netptr;
    pktA->curr = pktA->data;
    cap.caplen = USHRT_MAX;
    cap.type = SNOOP_FILTER_IPv4;
    failif(((SYSERR == snoopCapture(&cap, pktA))
            || (0 != cap.nmatch) || (mailboxCount(cap.queue) > 0)), "");

    testPrint(verbose, "Capture match");
    cap.type = SNOOP_FILTER_ALL;
    if (SYSERR == snoopCapture(&cap, pktA))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else if (1 != cap.nmatch)
    {
        failif(TRUE, "Packet did not match");
    }
    else if (mailboxCount(cap.queue) != 1)
    {
        failif(TRUE, "Packet not enqueued");
    }
    else
    {
        pktB = (struct packet *)mailboxReceive(cap.queue);
        failif((0 !=
                memcmp(pktB->data, pktA->data, phdr.caplen)),
               "Dequeued packet doesn't match");
    }

    testPrint(verbose, "Capture overrun");
    cap.type = SNOOP_FILTER_ALL;
    for (i = 0; i < SNOOP_QLEN; i++)
    {
        if (SYSERR == snoopCapture(&cap, pktA))
        {
            break;
        }
    }
    if (i < SNOOP_QLEN)
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        failif(((SYSERR != snoopCapture(&cap, pktA)) || (1 != cap.novrn)),
               "Packet did not overrun");
    }

    testPrint(verbose, "Close capture");
    failif((SYSERR == snoopClose(&cap)), "Returned SYSERR");

    /* TODO: RESUME HERE */

    netDown(ELOOP);
    close(ELOOP);

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }
#else /* NETHER */
    testSkip(TRUE, "");
#endif /* NETHER == 0 */
    return OK;
}
