/**
 * @file test_raw.c
 * @provides test_raw
 *
 * $Id: test_raw.c 2152 2010-01-07 02:43:18Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ethloop.h>
#include <ethernet.h>
#include <ipv4.h>
#include <raw.h>
#include <snoop.h>
#include <pcap.h>
#include <stdio.h>
#include <testsuite.h>

extern int _binary_data_testraw_pcap_start;
#define MAX_WAIT 10

/**
 * Tests raw sockets.
 * @return OK when testing is complete
 */
thread test_raw(bool verbose)
{
#if RAW0
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;
    device *devptr;
    struct raw *rawptr;
    struct netif *netptr;
    struct netaddr lip;
    struct netaddr rip;
    struct netaddr mask;
    struct packet *pkt;
    struct packet *pktA;
    struct pcap_pkthdr phdr;
    struct pcap_file_header pcap;
    uchar *data;
    uchar buf[500];
    int nproc;
    int wait;
    int i;

    lip.type = NETADDR_IPv4;
    lip.len = IPv4_ADDR_LEN;
    lip.addr[0] = 192;
    lip.addr[1] = 168;
    lip.addr[2] = 1;
    lip.addr[3] = 6;

    rip.type = NETADDR_IPv4;
    rip.len = IPv4_ADDR_LEN;
    rip.addr[0] = 192;
    rip.addr[1] = 168;
    rip.addr[2] = 1;
    rip.addr[3] = 1;

    mask.type = NETADDR_IPv4;
    mask.len = IPv4_ADDR_LEN;
    mask.addr[0] = 255;
    mask.addr[1] = 255;
    mask.addr[2] = 255;
    mask.addr[3] = 0;

    /* Initialization */
    testPrint(verbose, "Test case initialization");
    data = (uchar *)(&_binary_data_testraw_pcap_start);
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);
    if (SYSERR == open(ELOOP))
    {
        failif(TRUE, "");
    }
    else
    {
        if (SYSERR == netUp(ELOOP, &lip, &mask, NULL))
        {
            close(ELOOP);
            failif(TRUE, "");
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
            pkt = netGetbuf();
            failif(((NULL == netptr) || (SYSERR == (int)pkt)), "");
        }
    }
    if (!passed)
    {
        testFail(TRUE, "");
        return OK;
    }

    /* Test Open */
    testPrint(verbose, "Open RAW (Proto Only)");
    rawptr = NULL;
    if (SYSERR == open(RAW0, NULL, NULL, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        devptr = (device *)&devtab[RAW0];
        rawptr = &rawtab[devptr->minor];
        failif(((rawptr->state != RAW_ALLOC)
                || (rawptr->dev != devptr)
                || (rawptr->proto != IPv4_PROTO_ICMP)
                || (rawptr->localip.type != NULL)
                || (rawptr->remoteip.type != NULL)
                || (rawptr->icount != 0)
                || (rawptr->istart != 0)
                || (semcount(rawptr->isema) != 0)
                || (rawptr->flags != 0)), "Incorrect control block");
    }

    testPrint(verbose, "Open RAW (Already Open)");
    failif((SYSERR != open(RAW0, NULL, NULL, IPv4_PROTO_ICMP)),
           "Double open() succeeded");

    /* Test Control */
    testPrint(verbose, "Control (Closed Socket)");
    failif((control(RAW1, RAW_CTRL_SETFLAG, NULL, NULL) != SYSERR), "");

    testPrint(verbose, "Control (Bad Params)");
    failif((control(RAW0, -1, NULL, NULL) != SYSERR), "");

    testPrint(verbose, "Control (Set Flag)");
    failif(((NULL != control(RAW0, RAW_CTRL_SETFLAG,
                             (RAW_IACCEPT | RAW_IHDR), NULL))
            || (0 == (rawptr->flags & RAW_IACCEPT))
            || (0 == (rawptr->flags & RAW_IHDR))), "");

    testPrint(verbose, "Control (Clear Flag)");
    failif(((RAW_IHDR != control(RAW0, RAW_CTRL_CLRFLAG,
                                 RAW_IHDR, NULL))
            || (0 == (rawptr->flags & RAW_IACCEPT))
            || (1 == (rawptr->flags & RAW_IHDR))), "");

    /* Test Close */
    testPrint(verbose, "Close RAW");
    if (SYSERR == close(RAW0))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        devptr = (device *)&devtab[RAW0];
        rawptr = &rawtab[devptr->minor];
        failif(((rawptr->state != RAW_FREE)
                || (rawptr->dev != NULL)), "Control block not clear");
    }

    testPrint(verbose, "Close RAW (Already Closed)");
    failif((SYSERR != close(RAW0)), "Did not SYSERR");

    /* Test demux */
    testPrint(verbose, "Demulitplexing (No sockets)");
    failif((NULL != rawDemux(&rip, &lip, IPv4_PROTO_ICMP)), "");

    testPrint(verbose, "Demulitplexing (All Protos)");
    if ((SYSERR == open(RAW0, &lip, &rip, IPv4_PROTO_IGMP))
        || (SYSERR == open(RAW1, NULL, NULL, NULL)))
    {
        failif(TRUE, "Open failed");
    }
    else
    {
        devptr = (device *)&devtab[RAW1];
        rawptr = &rawtab[devptr->minor];
        if (rawptr != rawDemux(&rip, &lip, IPv4_PROTO_ICMP))
        {
            failif(TRUE, "Incorrect socket");
        }
        else
        {
            failif(((SYSERR == close(RAW0)) || (SYSERR == close(RAW1))),
                   "Close failed");
        }
    }

    testPrint(verbose, "Demulitplexing (Proto)");
    if ((SYSERR == open(RAW0, NULL, NULL, IPv4_PROTO_ICMP))
        || (SYSERR == open(RAW1, NULL, NULL, IPv4_PROTO_IGMP)))
    {
        failif(TRUE, "Open failed");
    }
    else
    {
        devptr = (device *)&devtab[RAW0];
        rawptr = &rawtab[devptr->minor];
        if (rawptr != rawDemux(&rip, &lip, IPv4_PROTO_ICMP))
        {
            failif(TRUE, "Incorrect socket");
        }
        else
        {
            failif(((SYSERR == close(RAW0)) || (SYSERR == close(RAW1))),
                   "Close failed");
        }
    }

    testPrint(verbose, "Demulitplexing (Remote IP)");
    if ((SYSERR == open(RAW0, NULL, NULL, IPv4_PROTO_ICMP))
        || (SYSERR == open(RAW1, NULL, &rip, IPv4_PROTO_ICMP)))
    {
        failif(TRUE, "Open failed");
    }
    else
    {
        devptr = (device *)&devtab[RAW1];
        rawptr = &rawtab[devptr->minor];
        if (rawptr != rawDemux(&rip, &lip, IPv4_PROTO_ICMP))
        {
            failif(TRUE, "Incorrect socket");
        }
        else
        {
            failif(((SYSERR == close(RAW0)) || (SYSERR == close(RAW1))),
                   "Close failed");
        }
    }

    testPrint(verbose, "Demulitplexing (Local IP)");
    if ((SYSERR == open(RAW0, NULL, &rip, IPv4_PROTO_ICMP))
        || (SYSERR == open(RAW1, &lip, &rip, IPv4_PROTO_ICMP)))
    {
        failif(TRUE, "Open failed");
    }
    else
    {
        devptr = (device *)&devtab[RAW1];
        rawptr = &rawtab[devptr->minor];
        if (rawptr != rawDemux(&rip, &lip, IPv4_PROTO_ICMP))
        {
            failif(TRUE, "Incorrect socket");
        }
        else
        {
            failif(((SYSERR == close(RAW0)) || (SYSERR == close(RAW1))),
                   "Close failed");
        }
    }

    /* Test Open */
    testPrint(verbose, "Open RAW (Full Spec)");
    if (SYSERR == open(RAW0, &lip, &rip, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        devptr = (device *)&devtab[RAW0];
        rawptr = &rawtab[devptr->minor];
        failif(((rawptr->state != RAW_ALLOC)
                || (rawptr->dev != devptr)
                || (rawptr->proto != IPv4_PROTO_ICMP)
                || (FALSE == netaddrequal(&lip, &rawptr->localip))
                || (FALSE == netaddrequal(&rip, &rawptr->remoteip))
                || (rawptr->icount != 0)
                || (rawptr->istart != 0)
                || (semcount(rawptr->isema) != 0)
                || (rawptr->flags != 0)), "Incorrect control block");
    }

    /* Test receive */
    testPrint(verbose, "Receive (Bad Params)");
    /* Get 1st packet */
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    pkt->len = phdr.caplen;
    memcpy(pkt->data, data, phdr.caplen);
    pkt->linkhdr = pkt->data;
    pkt->nethdr = pkt->linkhdr + ETH_HDR_LEN;
    pkt->curr = pkt->nethdr + IPv4_HDR_LEN;
    failif(((SYSERR != rawRecv(NULL, NULL, NULL, NULL))
            || (SYSERR != rawRecv(pkt, &rip, NULL, NULL))), "");

    testPrint(verbose, "Receive (No Match)");
    pktA = netGetbuf();
    memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
    pktA->linkhdr = pktA->data;
    pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
    pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
    if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_IGMP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        failif(((rawptr->icount != 0)
                || (semcount(rawptr->isema) != 0)
                || (TRUE == netaddrequal(&rawptr->src[0], &rip))
                || (rawptr->in[0] == pktA)), "Packet enqueued");

    }

    testPrint(verbose, "Receive (One Pkt)");
    pktA = netGetbuf();
    memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
    pktA->linkhdr = pktA->data;
    pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
    pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
    if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        failif(((rawptr->istart != 0)
                || (rawptr->icount != 1)
                || (semcount(rawptr->isema) != 1)
                || (FALSE == netaddrequal(&rawptr->src[0], &rip))
                || (rawptr->in[0] != pktA)), "Incorrectly enqueued");
    }

    testPrint(verbose, "Read (Closed socket)");
    failif((SYSERR != read(RAW1, buf, 500)), "");

    testPrint(verbose, "Read (One Pkt)");
    failif(((read(RAW0, buf, 500) != 8)
            || (memcmp(buf, (pkt->data + ETH_HDR_LEN + IPv4_HDR_LEN), 8)
                != 0)
            || (rawptr->icount != 0)
            || (rawptr->istart != 1)
            || (semcount(rawptr->isema) != 0)
            || (rawptr->in[0] != NULL)), "");

    testPrint(verbose, "Read (Include header)");
    pktA = netGetbuf();
    memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
    pktA->linkhdr = pktA->data;
    pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
    pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
    rawptr->flags = RAW_IHDR;
    if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Recv returned SYSERR");
    }
    else
    {
        failif(((read(RAW0, buf, 500) != (IPv4_HDR_LEN + 8))
                ||
                (memcmp(buf, (pkt->data + ETH_HDR_LEN), IPv4_HDR_LEN + 8)
                 != 0) || (rawptr->icount != 0) || (rawptr->istart != 2)
                || (semcount(rawptr->isema) != 0)
                || (rawptr->in[0] != NULL)), "");
    }


    testPrint(verbose, "Read (Accept)");
    pktA = netGetbuf();
    memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
    pktA->linkhdr = pktA->data;
    pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
    pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
    rawptr->flags = RAW_IACCEPT;
    rawptr->remoteip.type = NULL;
    if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Recv returned SYSERR");
    }
    else
    {
        failif(((read(RAW0, buf, 500) != 8)
                || (FALSE == netaddrequal(&rawptr->remoteip, &rip))), "");
    }

    testPrint(verbose, "Read (Small buf)");
    pktA = netGetbuf();
    memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
    pktA->linkhdr = pktA->data;
    pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
    pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
    if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Recv returned SYSERR");
    }
    else
    {
        failif(((read(RAW0, buf, 2) != 2)
                ||
                (memcmp(buf, (pkt->data + ETH_HDR_LEN + IPv4_HDR_LEN), 2)
                 != 0)), "");
    }

    testPrint(verbose, "Receive (Full buf)");
    for (i = 0; i < RAW_IBLEN; i++)
    {
        memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
        pktA->linkhdr = pktA->data;
        pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
        pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
        if (SYSERR == rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP))
        {
            break;
        }
    }
    if (i < RAW_IBLEN)
    {
        failif(TRUE, "Unable to fill buffer");
    }
    else
    {
        memcpy(pktA, pkt, sizeof(struct packet) + pkt->len);
        pktA->linkhdr = pktA->data;
        pktA->nethdr = pktA->linkhdr + ETH_HDR_LEN;
        pktA->curr = pktA->nethdr + IPv4_HDR_LEN;
        failif((SYSERR != rawRecv(pktA, &rip, &lip, IPv4_PROTO_ICMP)),
               "Did not return SYSERR");
    }

    testPrint(verbose, "Close RAW");
    failif((SYSERR == close(RAW0)), "");

    /* Test Write/Send */
    testPrint(verbose, "Open RAW");
    if (SYSERR == open(RAW0, &lip, NULL, IPv4_PROTO_ICMP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        devptr = (device *)&devtab[RAW0];
        rawptr = &rawtab[devptr->minor];
        failif(FALSE, "");
    }

    testPrint(verbose, "Send (Bad Params)");
    failif((rawSend(NULL, NULL, 0) != SYSERR), "");

    testPrint(verbose, "Send (Incomplete Spec)");
    failif((rawSend(rawptr, pkt->curr, 8) != SYSERR), "");

    testPrint(verbose, "Send (Incomplete Spec Hdr Inc)");
    rawptr->flags = RAW_OHDR;
    failif((rawSend(rawptr, pkt->curr, 8) != SYSERR), "");

    testPrint(verbose, "Send (Net Hdr Included)");
    /* Add ARP entry */
    /* Get 2nd Packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Get 3rd Packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    memcpy(pkt->data, data, phdr.caplen);
    pkt->len = phdr.caplen;
    pkt->linkhdr = pkt->data;
    pkt->nethdr = pkt->linkhdr + ETH_HDR_LEN;
    pkt->curr = pkt->nethdr + IPv4_HDR_LEN;
    /* Wait for ARP entry to be added */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(10);
    }
    if (MAX_WAIT == wait)
    {
        failif(MAX_WAIT, "ARP entry failed");
    }
    else
    {
        netaddrcpy(&rawptr->remoteip, &rip);
        control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
        if (SYSERR == rawSend(rawptr, pkt->nethdr, IPv4_HDR_LEN + 8))
        {
            failif(TRUE, "Returned SYSERR");
        }
        else
        {
            control(ELOOP, ELOOP_CTRL_GETHOLD, (long)buf, 500);
            failif((memcmp(pkt->data, buf, pkt->len) != 0),
                   "Incorrect Packet");
        }
    }

    testPrint(verbose, "Send");
    rawptr->flags = NULL;
    netaddrcpy(&rawptr->remoteip, &rip);
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    if (SYSERR == rawSend(rawptr, pkt->curr, 8))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        control(ELOOP, ELOOP_CTRL_GETHOLD, (long)buf, 500);
        failif((memcmp(pkt->data, buf, pkt->len) != 0),
               "Incorrect Packet");
    }

    testPrint(verbose, "Write");
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    if (SYSERR == write(RAW0, pkt->curr, 8))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        control(ELOOP, ELOOP_CTRL_GETHOLD, (long)buf, 500);
        failif((memcmp(pkt->data, buf, pkt->len) != 0),
               "Incorrect Packet");
    }

    close(RAW0);
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
#endif                          /* RAW0 */
    return OK;
}
