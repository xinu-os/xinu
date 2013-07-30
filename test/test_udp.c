/**
 * @file     test_udp.c
 * @provides test_udp
 *
 * $Id:
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ethloop.h>
#include <ipv4.h>
#include <interrupt.h>
#include <memory.h>
#include <network.h>
#include <snoop.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>
#include <thread.h>
#include <udp.h>

#if UDP1
//extern int _binary_data_testudp_pcap_start;
static struct packet *makePkt(ushort, ushort, struct netaddr *,
                              struct netaddr *, ushort, void *);
#endif

#define MAX_WAIT 10

/**
 * Tests UDP
 * @return OK when testing is complete
 */
thread test_udp(bool verbose)
{
#if UDP1
    struct udp *udpptr = NULL;
    ushort pta;
    ushort ptb;
    struct netaddr ipc;
    struct netaddr ipd;
    struct netaddr ipl;
    struct netaddr ipzero;
    struct packet *pkt[10];
    struct udpPkt *udppkt;
    uchar buffera[12];
    uchar bufferb[12];
    uchar bufferc[12];
    uchar bufferd[12];
    uchar bufferp[40];
    bool passed = TRUE;

    /*   struct pcap_pkthdr phdr;
       struct netif *netptr;
       int nproc;
       int i;
       int wait;
       struct packet *pktA;
       uchar buf[100];
       uchar *data;
       uchar *buffer;
     */
    pta = 20000;
    ptb = 30000;

    /* IP address "C" */
    ipc.type = NETADDR_IPv4;
    ipc.len = IPv4_ADDR_LEN;
    ipc.addr[0] = 192;
    ipc.addr[1] = 168;
    ipc.addr[2] = 1;
    ipc.addr[3] = 5;

    /* IP address "D" */
    ipd.type = NETADDR_IPv4;
    ipd.len = IPv4_ADDR_LEN;
    ipd.addr[0] = 192;
    ipd.addr[1] = 168;
    ipd.addr[2] = 1;
    ipd.addr[3] = 7;

    /* "Local" IP address */
    ipl.type = NETADDR_IPv4;
    ipl.len = IPv4_ADDR_LEN;
    ipl.addr[0] = 192;
    ipl.addr[1] = 168;
    ipl.addr[2] = 1;
    ipl.addr[3] = 8;

    /* Empty address */
    ipzero.type = 0;
    ipzero.len = 0;
    ipzero.addr[0] = 0;
    ipzero.addr[1] = 0;
    ipzero.addr[2] = 0;
    ipzero.addr[3] = 0;
    ipzero.addr[4] = 0;
    ipzero.addr[5] = 0;

    //struct ethloop *pelp;
    //irqmask im;

    enable();

    /* Test udpPkt structure */
    //testPrint(verbose, "Header structure");
    /* TODO: Figure out how this should be done */

    /* Test udpOpen */
    testPrint(verbose, "Open UDP devices (NULL local port)");
    if (SYSERR == open(UDP0, &ipl, &ipd, NULL, ptb))
    {
        failif(TRUE, "");
    }

    if (SYSERR == open(UDP1, &ipl, &ipc, NULL, pta))
    {
        failif(TRUE, "");
    }
    failif((udptab[0].localpt == udptab[1].localpt)
           || (udptab[0].localpt < UDP_PSTART)
           || (udptab[0].localpt > UDP_PMAX)
           || (udptab[1].localpt < UDP_PSTART)
           || (udptab[1].localpt > UDP_PMAX), "");
    if (SYSERR == close(UDP0))
    {
        failif(TRUE, "");
    }
    if (SYSERR == close(UDP1))
    {
        failif(TRUE, "");
    }

    /* Test udpOpen */
    testPrint(verbose, "Open UDP device (NULL remote port)");
    if (SYSERR == open(UDP0, &ipl, &ipd, pta, NULL))
    {
        failif(TRUE, "");
    }
    failif(udptab[0].remotept != NULL, "");
    if (SYSERR == close(UDP0))
    {
        failif(TRUE, "");
    }

    /* Test udpOpen */
    testPrint(verbose, "Open UDP device (NULL remote ip)");
    if (SYSERR == open(UDP0, &ipl, NULL, pta, ptb))
    {
        failif(TRUE, "");
    }
    failif(0 == netaddrequal(&udptab[0].remoteip, &ipzero), "");
    if (SYSERR == close(UDP0))
    {
        failif(TRUE, "");
    }


    /* Test udpOpen */
    testPrint(verbose, "Open UDP devices (same local port)");
    if (SYSERR == open(UDP0, &ipl, &ipd, pta, ptb))
    {
        failif(TRUE, "");
    }

    if (SYSERR == open(UDP1, &ipl, &ipd, pta, pta))
    {
        failif(TRUE, "");
    }
    failif(udptab[0].localpt != udptab[1].localpt, "");

    /* Only close the second UDP device this time, we want to test close
     * using UDP0 */
    if (SYSERR == close(UDP1))
    {
        failif(TRUE, "");
    }

    /* Test udpClose */
    testPrint(verbose, "Close UDP device");
    if (SYSERR == close(UDP0))
    {
        failif(TRUE, "");
    }
    failif((udptab[0].dev != 0) || (udptab[0].icount != 0)
           || (udptab[0].istart != 0) || (udptab[0].isem != 0)
           || (udptab[0].localpt != 0) || (udptab[0].remotept != 0)
           || (FALSE == netaddrequal(&udptab[0].localip, &ipzero))
           || (FALSE == netaddrequal(&udptab[0].remoteip, &ipzero))
           || (udptab[0].state != 0) || (udptab[0].flags != 0), "");

    /* Test udpControl */
    testPrint(verbose, "UDP Control: Binding");
    /* Open UDP device to resume testing of that device */
    if (SYSERR == open(UDP0, &ipl, NULL, NULL, NULL))
    {
        failif(TRUE, "");
    }
    control(UDP0, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP0, UDP_CTRL_BIND, ptb, (long)&ipc);
    failif((udptab[0].localpt != pta)
           || (FALSE == netaddrequal(&udptab[0].localip, &ipl))
           || (udptab[0].remotept != ptb)
           || (FALSE == netaddrequal(&udptab[0].remoteip, &ipc)), "");

    /* Test udpControl */
    testPrint(verbose, "UDP Control: Flags");
    control(UDP0, UDP_CTRL_SETFLAG, UDP_FLAG_INCHDR | UDP_FLAG_NOBLOCK
            | UDP_FLAG_BINDFIRST, NULL);
    control(UDP0, UDP_CTRL_CLRFLAG, UDP_FLAG_NOBLOCK, NULL);
    /* At this point NOBLOCK should be the only flag that is off */
    failif((FALSE == (udptab[0].flags & UDP_FLAG_INCHDR))
           || (udptab[0].flags & UDP_FLAG_NOBLOCK)
           || (FALSE == (udptab[0].flags & UDP_FLAG_BINDFIRST)), "");

    /* Test udpDemux */
    testPrint(verbose, "UDP Demux (2 sockets)");
    open(UDP1, &ipl, NULL, pta, NULL);
    udpptr = udpDemux(pta, ptb, &ipl, &ipc);
    failif((udpptr == &udptab[1]) || (NULL == udpptr), "");

    /* Test udpRecv and udpRead */
    testPrint(verbose, "Receive and read UDP packets");

    control(UDP0, UDP_CTRL_CLRFLAG, UDP_FLAG_INCHDR
            | UDP_FLAG_BINDFIRST, NULL);

    control(UDP0, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP0, UDP_CTRL_BIND, ptb, (long)&ipc);

    pkt[0] = makePkt(ptb, pta, &ipl, &ipc, 5, "ABCDE");
    pkt[1] = makePkt(ptb, pta, &ipl, &ipc, 4, "FGHI");
    pkt[2] = makePkt(ptb, pta, &ipl, &ipc, 3, "JKL");
    pkt[3] = makePkt(ptb, pta, &ipl, &ipc, 2, "MN");
    if (SYSERR == udpRecv(pkt[0], &ipc, &ipl))
    {
        failif(TRUE, "");
    }
    if (SYSERR == udpRecv(pkt[1], &ipc, &ipl))
    {
        failif(TRUE, "");
    }
    if (SYSERR == udpRecv(pkt[2], &ipc, &ipl))
    {
        failif(TRUE, "");
    }
    if (SYSERR == udpRecv(pkt[3], &ipc, &ipl))
    {
        failif(TRUE, "");
    }
    if (SYSERR == read(UDP0, buffera, 5))
    {
        failif(TRUE, "reada");
    }
    if (SYSERR == read(UDP0, bufferb, 5))
    {
        failif(TRUE, "readb");
    }
    if (SYSERR == read(UDP0, bufferc, 5))
    {
        failif(TRUE, "readb");
    }
    if (SYSERR == read(UDP0, bufferd, 5))
    {
        failif(TRUE, "readb");
    }
    failif((0 != strncmp((char *)buffera, "ABCDE", 5))
           || (0 != strncmp((char *)bufferb, "FGHI", 4))
           || (0 != strncmp((char *)bufferc, "JKL", 3))
           || (0 != strncmp((char *)bufferd, "MN", 2)), "");

    /* Test udpRecv and udpRead */
    testPrint(verbose, "Two UDP sockets");
    pkt[0] = makePkt(pta, pta, &ipc, &ipl, 9, "ABCDEFGHI");
    udpRecv(pkt[0], &ipc, &ipl);
    read(UDP1, bufferc, 9);
    failif(0 != strncmp((char *)bufferc, "ABCDEFGHI", 9), "");

    /* Test udpRecv and udpRead */
    testPrint(verbose, "Receive and read UDP packets (again)");

    control(UDP0, UDP_CTRL_CLRFLAG, UDP_FLAG_INCHDR
            | UDP_FLAG_BINDFIRST, NULL);

    control(UDP0, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP0, UDP_CTRL_BIND, ptb, (long)&ipc);

    pkt[0] = makePkt(ptb, pta, &ipl, &ipc, 5, "OPQRS");
    pkt[1] = makePkt(ptb, pta, &ipl, &ipc, 3, "TUV");
    pkt[2] = makePkt(ptb, pta, &ipl, &ipc, 2, "WX");
    pkt[3] = makePkt(ptb, pta, &ipl, &ipc, 2, "YZ");
    udpRecv(pkt[0], &ipc, &ipl);
    udpRecv(pkt[1], &ipc, &ipl);
    udpRecv(pkt[2], &ipc, &ipl);
    udpRecv(pkt[3], &ipc, &ipl);
    if (SYSERR == read(UDP0, buffera, 5))
    {
        failif(TRUE, "reada");
    }
    if (SYSERR == read(UDP0, bufferb, 3))
    {
        failif(TRUE, "readb");
    }
    if (SYSERR == read(UDP0, bufferc, 2))
    {
        failif(TRUE, "readb");
    }
    if (SYSERR == read(UDP0, bufferd, 2))
    {
        failif(TRUE, "readb");
    }
    failif(0 != strncmp((char *)buffera, "OPQRS", 5)
           || (0 != strncmp((char *)bufferb, "TUV", 3))
           || (0 != strncmp((char *)bufferc, "WX", 2))
           || (0 != strncmp((char *)bufferd, "YZ", 2)), "");

    /* Test udpRecv and udpRead on multiple sockets */
    testPrint(verbose, "Recv/read with varying sockets (1)");

    /* Test 1 */
    control(UDP0, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP0, UDP_CTRL_BIND, NULL, NULL);
    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 5, "test1");
    udpRecv(pkt[0], &ipc, &ipl);
    read(UDP0, buffera, 5);
    failif(strncmp((char *)buffera, "test1", 5), "");

    /* Test 2 */
    testPrint(verbose, "Recv/read socket test 2");
    control(UDP0, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP0, UDP_CTRL_BIND, ptb, NULL);

    control(UDP1, UDP_CTRL_ACCEPT, pta, (long)&ipl);
    control(UDP1, UDP_CTRL_BIND, 0, NULL);

    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 6, "test2a");
    pkt[1] = makePkt(pta, pta, &ipc, &ipl, 6, "test2b");
    udpRecv(pkt[0], &ipc, &ipl);
    udpRecv(pkt[1], &ipc, &ipl);
    read(UDP0, buffera, 6);
    read(UDP1, bufferb, 6);

    failif(0 != strncmp((char *)buffera, "test2a", 6)
           || (0 != strncmp((char *)bufferb, "test2b", 6)), "");

    /* Test 3 */
    testPrint(verbose, "Recv/read socket test 3");
    control(UDP0, UDP_CTRL_BIND, ptb, (long)&ipc);
    control(UDP1, UDP_CTRL_BIND, ptb, NULL);

    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 6, "test3a");
    pkt[1] = makePkt(ptb, pta, &ipd, &ipl, 6, "test3b");
    udpRecv(pkt[0], &ipc, &ipl);
    udpRecv(pkt[1], &ipd, &ipl);
    read(UDP0, buffera, 6);
    read(UDP1, bufferb, 6);

    failif((0 != strncmp((char *)buffera, "test3a", 6))
           || (0 != strncmp((char *)bufferb, "test3b", 6)), "");

    /* Test 4 */
    testPrint(verbose, "Recv/read socket test 4");
    control(UDP0, UDP_CTRL_BIND, ptb, (long)&ipc);
    control(UDP1, UDP_CTRL_BIND, ptb, (long)&ipd);

    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 6, "test4a");
    pkt[1] = makePkt(ptb, pta, &ipd, &ipl, 6, "test4b");
    udpRecv(pkt[0], &ipc, &ipl);
    udpRecv(pkt[1], &ipd, &ipl);
    read(UDP0, buffera, 6);
    read(UDP1, bufferb, 6);

    failif((0 != strncmp((char *)buffera, "test4a", 6))
           || (0 != strncmp((char *)bufferb, "test4b", 6)), "");

    /* Test 5 */
    testPrint(verbose, "Recv/read socket test 5");
    control(UDP0, UDP_CTRL_BIND, 0, NULL);
    control(UDP1, UDP_CTRL_ACCEPT, ptb, (long)&ipl);
    control(UDP1, UDP_CTRL_BIND, 0, NULL);

    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 6, "test5a");
    pkt[1] = makePkt(pta, ptb, &ipc, &ipl, 6, "test5b");
    udpRecv(pkt[0], &ipc, &ipl);
    udpRecv(pkt[1], &ipc, &ipl);
    read(UDP0, buffera, 6);
    read(UDP1, bufferb, 6);

    failif((0 != strncmp((char *)buffera, "test5a", 6))
           || (0 != strncmp((char *)bufferb, "test5b", 6)), "");

    /* Read entire UDP packet */
    testPrint(verbose, "Read entire UDP packet");
    control(UDP0, UDP_CTRL_SETFLAG, UDP_FLAG_INCHDR, NULL);
    pkt[0] = makePkt(ptb, pta, &ipc, &ipl, 6, "inchdr");
    udpRecv(pkt[0], &ipc, &ipl);
    read(UDP0, bufferp, UDP_HDR_LEN + 6);
    udppkt = (struct udpPkt *)bufferp;
    failif((udppkt->srcPort != ptb) || (udppkt->dstPort != pta)
           || (udppkt->len != UDP_HDR_LEN + 6)
           || (0 != strncmp((char *)udppkt->data, "inchdr", 6)), "");

    /* Done testing, attempt to close all UDP devices (MAKE SURE BOTH
     * DEVICES ARE OPEN BEFORE YOU CLOSE THEM!!!) */
    close(UDP0);
    close(UDP1);

    /* Print out the overall test's status (pass or fail) */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }
#endif                          /* UDP1 */
    return OK;
}

/* Same code as udpSend except it returns the pointer to the packet
 * instead of sending the packet over IP */
#if UDP1
static struct packet *makePkt(ushort localpt, ushort remotept,
                              struct netaddr *localip,
                              struct netaddr *remoteip, ushort datalen,
                              void *buf)
{
    struct packet *pkt;
    struct udpPkt *udppkt = NULL;

    pkt = netGetbuf();

    if (SYSERR == (int)pkt)
    {
        return (struct packet *)SYSERR;
    }

    /* Set the length of the packet and set the curr pointer back that 
     * length */
    pkt->len = UDP_HDR_LEN + datalen;
    /* Round the datalength to maintain word alignment */
    pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

    /* Set UDP header fields and fill the packet with the data */
    udppkt = (struct udpPkt *)(pkt->curr);
    udppkt->srcPort = hs2net(localpt);
    udppkt->dstPort = hs2net(remotept);
    udppkt->len = hs2net(pkt->len);
    udppkt->chksum = 0;

    /* Zero out the data buffer so empty space is padded with zeroes */
    bzero(udppkt->data, datalen);
    memcpy(udppkt->data, buf, datalen);

    udppkt->chksum = udpChksum(pkt, UDP_HDR_LEN + datalen,
                               localip, remoteip);

    return pkt;
}
#endif                          /* UDP1 */
