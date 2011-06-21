/**
 * @file     test_netif.c
 * @provides test_netif
 *
 * $Id: test_netif.c 2152 2010-01-07 02:43:18Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ethloop.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <snoop.h>
#include <pcap.h>
#include <testsuite.h>
#include <thread.h>

#ifndef ELOOP
#define ELOOP (-1)
#endif

#ifndef NNETIF
#define NNETIF (-1)
#endif

extern int _binary_data_testnetif_pcap_start;

/**
 * Tests the network interfaces.
 * @return OK when testing is complete
 */
thread test_netif(bool verbose)
{
    bool passed = TRUE;
    int i = 0;
    struct netaddr ip;
    struct netaddr mask;
    struct netaddr gate;
    struct netaddr hw;
    struct netaddr brc;
    struct pcap_file_header pcap;
    struct pcap_pkthdr phdr;
    struct packet *pkt;
    struct netif *netptr;
    uchar *data;

    enable();

    ip.type = NETADDR_IPv4;
    ip.len = IPv4_ADDR_LEN;
    ip.addr[0] = 192;
    ip.addr[1] = 168;
    ip.addr[2] = 1;
    ip.addr[3] = 6;

    mask.type = NETADDR_IPv4;
    mask.len = IPv4_ADDR_LEN;
    mask.addr[0] = 255;
    mask.addr[1] = 255;
    mask.addr[2] = 255;
    mask.addr[3] = 0;

    gate.type = NETADDR_IPv4;
    gate.len = IPv4_ADDR_LEN;
    gate.addr[0] = 192;
    gate.addr[1] = 168;
    gate.addr[2] = 1;
    gate.addr[3] = 1;

    brc.type = NETADDR_IPv4;
    brc.len = IPv4_ADDR_LEN;
    brc.addr[0] = 255;
    brc.addr[1] = 255;
    brc.addr[2] = 255;
    brc.addr[3] = 255;

    testPrint(verbose, "Open loopback ethernet device");
    failif((SYSERR == open(ELOOP)), "");

    testPrint(verbose, "Start network interface");
    if (SYSERR == netUp(ELOOP, &ip, &mask, &gate))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        for (i = 0; i < NNETIF; i++)
        {
            if ((NET_ALLOC == netiftab[i].state)
                && (ELOOP == netiftab[i].dev))
            {
                break;
            }
        }

        netptr = &netiftab[i];
        if ((i >= NNETIF)
            || (netptr->state != NET_ALLOC)
            || (netptr->mtu != ELOOP_MTU)
            || (netptr->linkhdrlen != ELOOP_LINKHDRSIZE)
            || (FALSE == netaddrequal(&netptr->ip, &ip))
            || (FALSE == netaddrequal(&netptr->mask, &mask))
            || (FALSE == netaddrequal(&netptr->gateway, &gate)))
        {
            failif(TRUE, "Incorrect structure setup");
        }
        else
        {
            for (i = 0; i < NET_NTHR; i++)
            {
                if (isbadtid(netptr->recvthr[i]))
                {
                    failif(TRUE, "Bad recvthr");
                    break;
                }
            }
            if (NET_NTHR == i)
            {
                failif(FALSE, "");
            }
        }
    }

    testPrint(verbose, "Stop network interface");
    failif((SYSERR == netDown(ELOOP)), "");

    testPrint(verbose, "Start network interface (no gateway)");
    failif((SYSERR == netUp(ELOOP, &ip, &mask, NULL)), "");
    for (i = 0; i < NNETIF; i++)
    {
        if ((NET_ALLOC == netiftab[i].state)
            && (ELOOP == netiftab[i].dev))
        {
            break;
        }
    }

    /* Kill receiver threads */
    netptr = &netiftab[i];
    for (i = 0; i < NET_NTHR; i++)
    {
        kill(netptr->recvthr[i]);
        netptr->recvthr[i] = BADTID;
    }

    testPrint(verbose, "Get packet buffer");
    pkt = netGetbuf();
    failif((SYSERR == (int)pkt), "");

    data = (uchar *)&_binary_data_testnetif_pcap_start;
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    pkt->nif = netptr;
    pkt->len = phdr.caplen - netptr->linkhdrlen;
    pkt->curr = pkt->curr - pkt->len;
    memcpy(pkt->curr, data + netptr->linkhdrlen, pkt->len);

    hw.type = NETADDR_ETHERNET;
    hw.len = ETH_ADDR_LEN;
    hw.addr[0] = 0xAA;
    hw.addr[1] = 0xBB;
    hw.addr[2] = 0xCC;
    hw.addr[3] = 0xDD;
    hw.addr[4] = 0xEE;
    hw.addr[5] = 0xAA;

    testPrint(verbose, "Send packet (known hardware address)");
    if (SYSERR == netSend(pkt, &hw, NULL, ETHER_TYPE_ARP))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        if ((SYSERR == read(netptr->dev, pkt, phdr.caplen))
            || (memcmp(pkt, data, phdr.caplen) != 0))
        {
            failif(TRUE, "Packet did not match");
        }
        else
        {
            failif(FALSE, "");
        }
    }

    testPrint(verbose, "Free packet buffer");
    failif((SYSERR == netFreebuf(pkt)), "");

    testPrint(verbose, "Stop network interface");
    failif(((SYSERR == netDown(ELOOP)) || (SYSERR == close(ELOOP))), "");

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
