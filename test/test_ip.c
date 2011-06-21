/**
 * @file test_ip.c
 * @ provides test_ip.c
 *
 * $Id: test_ip.c 2152 2010-01-07 02:43:18Z brylow $
 */

#include <stddef.h>
#include <device.h>
#include <ethloop.h>
#include <ipv4.h>
#include <snoop.h>
#include <pcap.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>
#include <thread.h>
#include <udp.h>

extern int _binary_data_testip_pcap_start;
#define MAX_WAIT 10

#ifndef ELOOP
#define ELOOP (-1)
#endif

#ifndef NNETIF
#define NNETIF (-1)
#endif

thread test_ip(bool verbose)
{
    struct netaddr dst;
    struct netaddr src;
    struct netaddr mask;
    struct netif *netptr;
    struct pcap_file_header pcap;
    struct pcap_pkthdr phdr;
    struct packet *pktA;
    struct packet *pktB;
    uchar *data;
    uchar buf[500];
    int i;
    int nproc;
    int wait;
    bool passed = TRUE;

    src.type = NETADDR_IPv4;
    src.len = IPv4_ADDR_LEN;
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
    mask.type = NETADDR_IPv4;
    mask.len = IPv4_ADDR_LEN;

    src.addr[0] = 192;
    src.addr[1] = 168;
    src.addr[2] = 1;
    src.addr[3] = 6;

    dst.addr[0] = 192;
    dst.addr[1] = 168;
    dst.addr[2] = 1;
    dst.addr[3] = 1;

    mask.addr[0] = 255;
    mask.addr[1] = 255;
    mask.addr[2] = 255;
    mask.addr[3] = 0;

    testPrint(verbose, "Initialization");
    sleep(500);
    if (SYSERR == open(ELOOP))
    {
        failif(TRUE, "Open returned SYSERR");
    }
    else
    {
        if (SYSERR == netUp(ELOOP, &src, &mask, &dst))
        {
            failif(TRUE, "netUp returned SYSERR");
        }
        else
        {
            netptr = NULL;
            for (i = 0; i < NNETIF; i++)
            {
                if (ELOOP == netiftab[i].dev)
                {
                    netptr = &netiftab[i];
                    break;
                }
            }
            pktA = netGetbuf();
            pktB = netGetbuf();
            failif(((NULL == netptr)
                    || (SYSERR == (int)pktA)
                    || (SYSERR == (int)pktB)),
                   "Buffer allocation failed");
        }
    }
    if (!passed)
    {
        testFail(TRUE, "");
        return OK;
    }

    data = (uchar *)(&_binary_data_testip_pcap_start);
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);

    testPrint(verbose, "Ipv4Send");
    /* Get 1st Packet */
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(10);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Get 2nd Packet */
        data += phdr.caplen;
        memcpy(&phdr, data, sizeof(phdr));
        data += sizeof(phdr);
        if (PCAP_MAGIC != pcap.magic)
        {
            phdr.caplen = endswap(phdr.caplen);
        }
        memcpy(pktA->data, data, phdr.caplen);
        pktA->len = phdr.caplen;
        pktA->nif = netptr;
        pktA->linkhdr = pktA->data;
        pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
        pktA->curr = pktA->nethdr + IPv4_HDR_LEN;

        pktB->curr -= UDP_HDR_LEN + 4;
        pktB->len += UDP_HDR_LEN + 4;
        memcpy(pktB->curr, pktA->curr, pktB->len);

        control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);

        if (OK != ipv4Send(pktB, &src, &dst, IPv4_PROTO_UDP))
        {
            failif(TRUE, "ipv4Send didn't return okay");
        }
        else
        {
            control(ELOOP, ELOOP_CTRL_GETHOLD, (long)buf, 500);
            failif(0 != memcmp(buf, pktA->linkhdr, pktA->len), "");
        }
    }

    /* ipv4Recv Testing */
    //TODO: Finish ipv4Recv
/*	testPrint(verbose, "ipv4Recv");
	pktA->curr -= IPv4_HDR_LEN;

	if (SYSERR == rawOpen(RAW0, IPv4_PROTO_UDP, ))
    {
        failif(TRUE, "Open returned SYSERR");
    }

	

	if (OK != ipv4Recv(pktA))
	{
		failif(TRUE, "Didn't return okay");
	}
	else
	{
	}
*/
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

    return OK;
}
