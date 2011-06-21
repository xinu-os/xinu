/**
 * @file     test_arp.c
 * @provides test_arp
 *
 * $Id: test_arp.c 2152 2010-01-07 02:43:18Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <clock.h>
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

extern int _binary_data_testarp_pcap_start;
#define MAX_WAIT 10

static thread notifyTest(void)
{
    while (ARP_MSG_RESOLVED != receive());
    return OK;
}

static thread freeTest(void)
{
    while (TIMEOUT != receive());
    return OK;
}

static thread lookupTest(uchar *request, int rqlen, uchar *reply,
                         int rplen)
{
    uchar buf[ELOOP_BUFSIZE];
    control(ELOOP, ELOOP_CTRL_GETHOLD, (int)buf, ELOOP_BUFSIZE);
    if (memcmp(buf, request, rqlen) != 0)
    {
        return SYSERR;
    }
    write(ELOOP, reply, rplen);
    return OK;
}

/**
 * Tests ARP.
 * @return OK when testing is complete
 */
thread test_arp(bool verbose)
{
    bool passed = TRUE;
    int i = 0;
    struct netaddr ip;
    struct netaddr mask;
    struct netaddr praddr;
    struct netaddr hwaddr;
    struct netaddr addrbuf;
    struct pcap_file_header pcap;
    struct pcap_pkthdr phdr;
    struct packet *pkt;
    struct netif *netptr;
    struct ethloop *pelp;
    uchar *data;
    uchar *request;
    struct arpPkt *arp;
    struct arpEntry *entry;
    uchar buf[ELOOP_BUFSIZE];
    int nproc;
    int nout;
    int wait;
    tid_typ tids[ARP_NTHRWAIT];
    tid_typ tid;
    irqmask im;

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

    /* Initialize loopback ethernet and network interface */
    testPrint(verbose, "Test case initialization");

    data = (uchar *)(&_binary_data_testarp_pcap_start);
    memcpy(&pcap, data, sizeof(pcap));
    data += sizeof(pcap);

    if (SYSERR == open(ELOOP))
    {
        failif(TRUE, "");
    }
    else
    {
        failif((SYSERR == netUp(ELOOP, &ip, &mask, NULL)), "");
    }
    if (!passed)
    {
        testFail(TRUE, "");
        return OK;
    }

    /* Setup pointers to underlying structures */
#if NNETIF
    for (i = 0; i < NNETIF; i++)
    {
        if ((NET_ALLOC == netiftab[i].state)
            && (ELOOP == netiftab[i].dev))
        {
            break;
        }
    }
#endif
    netptr = &netiftab[i];
    pelp = &elooptab[devtab[ELOOP].minor];

    pkt = netGetbuf();
    if ((int)pkt != SYSERR)
    {
        pkt->nif = netptr;
    }

    praddr.type = NETADDR_IPv4;
    praddr.len = IPv4_ADDR_LEN;
    praddr.addr[0] = 192;
    praddr.addr[1] = 168;
    praddr.addr[2] = 1;
    praddr.addr[3] = 3;

    hwaddr.type = NETADDR_ETHERNET;
    hwaddr.len = ETH_ADDR_LEN;
    hwaddr.addr[0] = 0xAA;
    hwaddr.addr[1] = 0xBB;
    hwaddr.addr[2] = 0xCC;
    hwaddr.addr[3] = 0xDD;
    hwaddr.addr[4] = 0xEE;
    hwaddr.addr[5] = 0xFF;

    /* Test arpPkt structure */
    testPrint(verbose, "Header structure (Request)");
    /* Get 1st packet */
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    pkt = netGetbuf();
    pkt->len +=
        ARP_CONST_HDR_LEN + (2 * IPv4_ADDR_LEN) + (2 * ETH_ADDR_LEN);
    pkt->curr -= pkt->len;
    arp = (struct arpPkt *)pkt->curr;
    arp->hwtype = hs2net(ARP_HWTYPE_ETHERNET);
    arp->prtype = hs2net(ARP_PRTYPE_IPv4);
    arp->hwalen = ETH_ADDR_LEN;
    arp->pralen = IPv4_ADDR_LEN;
    arp->op = hs2net(ARP_OP_RQST);
    memcpy(&arp->addrs[ARP_ADDR_SHA(arp)], netptr->hwaddr.addr,
           arp->hwalen);
    memcpy(&arp->addrs[ARP_ADDR_SPA(arp)], netptr->ip.addr, arp->pralen);
    memcpy(&arp->addrs[ARP_ADDR_DPA(arp)], praddr.addr, arp->pralen);
    failif((0 != memcmp(data + ELOOP_LINKHDRSIZE, arp, pkt->len)), "");

    /* Test arpPkt structure */
    testPrint(verbose, "Header structure (Reply)");
    /* Get 2nd packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    arp->op = hs2net(ARP_OP_REPLY);
    memcpy(&arp->addrs[ARP_ADDR_DHA(arp)], &arp->addrs[ARP_ADDR_SHA(arp)],
           arp->hwalen);
    arp->addrs[ARP_ADDR_DHA(arp) + ETH_ADDR_LEN - 1] = 0xCC;
    memcpy(&arp->addrs[ARP_ADDR_DPA(arp)], praddr.addr, arp->pralen);
    memcpy(&arp->addrs[ARP_ADDR_SHA(arp)], netptr->hwaddr.addr,
           arp->hwalen);
    memcpy(&arp->addrs[ARP_ADDR_SPA(arp)], netptr->ip.addr, arp->pralen);
    failif((0 != memcmp(data + ELOOP_LINKHDRSIZE, arp, pkt->len)), "");

    /* Test arpAlloc, free entry exists */
    testPrint(verbose, "Allocate free entry");
    /* Make first entry used */
    arptab[0].state = ARP_USED;
    arptab[0].expires = clktime + ARP_TTL_UNRESOLVED;
    entry = arpAlloc();
    failif(((NULL == entry) || (entry == &arptab[0])
            || (0 == (entry->state & ARP_USED))), "");

    /* Test arpAlloc, free entry exists */
    testPrint(verbose, "Allocate used entry");
    arptab[1].state = ARP_USED;
    arptab[1].expires = clktime + 1;
    /* Make all entries (except the first 2) have ARP_TTL_RESOLVED */
    for (i = 2; i < ARP_NENTRY; i++)
    {
        arptab[i].state = ARP_USED;
        arptab[i].expires = clktime + ARP_TTL_RESOLVED;
    }
    entry = arpAlloc();
    failif(((NULL == entry) || (entry != &arptab[1])
            || (0 == (entry->state & ARP_USED))), "");

    /* Test arpNotify */
    testPrint(verbose, "Notify waiting threads (bad params)");
    failif((SYSERR != arpNotify(NULL, TIMEOUT)), "");

    /* Test arpNotify */
    testPrint(verbose, "Notify waiting threads");
    entry = &arptab[0];
    entry->state = ARP_UNRESOLVED;
    for (i = 0; i < ARP_NTHRWAIT; i++)
    {
        tid =
            create((void *)notifyTest, INITSTK, INITPRIO, "notifyTest",
                   0);
        tids[i] = tid;
        entry->waiting[i] = tid;
        entry->count++;
        ready(tid, RESCHED_NO);
    }
    recvclr();
    arpNotify(entry, ARP_MSG_RESOLVED);
    nout = FALSE;
    nproc = 0;
    tid = recvtime(100);
    while ((tid != TIMEOUT) && (nproc < ARP_NTHRWAIT))
    {
        for (i = 0; i < ARP_NTHRWAIT; i++)
        {
            if (tid == tids[i])
            {
                tids[i] = NULL;
            }
        }
        nproc++;
        tid = recvtime(100);
    }
    for (i = 0; i < ARP_NTHRWAIT; i++)
    {
        if (tids[i] != NULL)
        {
            kill(tids[i]);
            nout = TRUE;
        }
    }
    failif(nout || (entry->count != 0), "");

    /* Test arpFree */
    testPrint(verbose, "Free entry (bad params)");
    failif((SYSERR != arpFree(NULL)), "");

    /* Test arpFree */
    testPrint(verbose, "Free resolved entry");
    entry = &arptab[0];
    entry->state = ARP_RESOLVED;
    entry->expires = clktime;
    failif(((SYSERR == arpFree(entry)) || (entry->expires != 0)), "");

    /* Test arpFree */
    testPrint(verbose, "Free unresolved entry");
    entry = &arptab[0];
    entry->state = ARP_UNRESOLVED;
    for (i = 0; i < ARP_NTHRWAIT; i++)
    {
        tid = create((void *)freeTest, INITSTK, INITPRIO, "freeTest", 0);
        tids[i] = tid;
        entry->waiting[i] = tid;
        entry->count++;
        ready(tid, RESCHED_NO);
    }
    recvclr();
    if (SYSERR == arpFree(entry))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        nout = FALSE;
        nproc = 0;
        tid = recvtime(100);
        while ((tid != TIMEOUT) && (nproc < ARP_NTHRWAIT))
        {
            for (i = 0; i < ARP_NTHRWAIT; i++)
            {
                if (tid == tids[i])
                {
                    tids[i] = NULL;
                }
            }
            nproc++;
            tid = recvtime(100);
        }
        for (i = 0; i < ARP_NTHRWAIT; i++)
        {
            if (tids[i] != NULL)
            {
                kill(tids[i]);
                nout = TRUE;
            }
        }
        failif(nout || (entry->count != 0), "");
    }

    /* Test arpGetEntry */
    testPrint(verbose, "Get entry");
    for (i = 0; i < ARP_NENTRY; i++)
    {
        arpFree(&arptab[i]);
    }
    nout = 4;
    if (ARP_NENTRY < nout)
    {
        nout = ARP_NENTRY;
    }
    for (i = 1; i < nout; i++)
    {
        entry = &arptab[i];
        entry->state = ARP_RESOLVED;
        entry->nif = netptr;
        praddr.addr[3] = i + 1;
        hwaddr.addr[5] = ((i + 0xA) << 4) + (i + 0xA);
        netaddrcpy(&entry->hwaddr, &hwaddr);
        netaddrcpy(&entry->praddr, &praddr);
        entry->expires = clktime + ARP_TTL_RESOLVED;
    }
    for (i = 1; i < nout; i++)
    {
        praddr.addr[3] = i + 1;
        entry = arpGetEntry(&praddr);
        if (entry != &arptab[i])
        {
            break;
        }
    }
    failif((i != nout), "");

    /* Test arpGetEntry with timeout */
    testPrint(verbose, "Get entry (timeout entries)");
    arptab[i].expires = clktime - 1;
    praddr.addr[3] = 2;
    entry = arpGetEntry(&praddr);
    if (entry != &arptab[1])
    {
        failif(TRUE, "Incorrect entry");
    }
    else
    {
        failif((arptab[0].state != ARP_FREE),
               "Did not free expired entry");
    }
    for (i = 0; i < ARP_NENTRY; i++)
    {
        arpFree(&arptab[i]);
    }

    /* Test receive reply for non-existing ARP table entry */
    testPrint(verbose, "Receive ARP reply, new entry");
    /* Get 3rd packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    praddr.addr[3] = 3;
    hwaddr.addr[5] = 0xCC;
    nout = pelp->nout;
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Wait until packet is processed */
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
        /* Check entry and ensure reply was not sent */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if (ARP_RESOLVED == arptab[i].state)
            {
                entry = &arptab[i];
                break;
            }
        }
        if (NULL == entry)
        {
            failif(TRUE, "No entry inserted");
        }
        else if ((FALSE == netaddrequal(&praddr, &entry->praddr))
                 || (FALSE == netaddrequal(&hwaddr, &entry->hwaddr))
                 || (entry->nif != netptr) || (entry->count != 0))
        {
            failif(TRUE, "Entry incorrect");
        }
        else if (pelp->nout > (nout + 1))
        {
            failif(TRUE, "Reply sent to reply");
        }
        else
        {
            failif(FALSE, "");
        }
    }

    /* Test receive request for non-existing ARP table entry */
    testPrint(verbose, "Receive ARP request, new entry");
    /* Get 4th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    praddr.addr[3] = 1;
    hwaddr.addr[5] = 0xAA;
    nout = pelp->nout;
    nproc = netptr->nproc;
    im = disable();
    write(ELOOP, data, phdr.caplen);
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    restore(im);
    /* Wait until packet is processed */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(100);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Check entry and ensure reply was not sent */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if ((ARP_RESOLVED == arptab[i].state)
                && (netaddrequal(&praddr, &arptab[i].praddr)))
            {
                entry = &arptab[i];
                break;
            }
        }
        if (NULL == entry)
        {
            failif(TRUE, "No entry inserted");
        }
        else if ((FALSE == netaddrequal(&praddr, &entry->praddr))
                 || (FALSE == netaddrequal(&hwaddr, &entry->hwaddr))
                 || (entry->nif != netptr) || (entry->count != 0))
        {
            failif(TRUE, "Entry incorrect");
        }
        else
        {
            control(ELOOP, ELOOP_CTRL_GETHOLD, (int)buf, ELOOP_BUFSIZE);
            /* Get 5th packet */
            data += phdr.caplen;
            memcpy(&phdr, data, sizeof(phdr));
            data += sizeof(phdr);
            if (PCAP_MAGIC != pcap.magic)
            {
                phdr.caplen = endswap(phdr.caplen);
            }
            failif((memcmp(data, buf, phdr.caplen) != 0),
                   "Invalid reply");
        }
    }

    /* Test receive request for non-supported hardware type */
    testPrint(verbose, "Receive ARP request, bad HW type");
    /* Get 6th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    praddr.addr[3] = 2;
    hwaddr.addr[5] = 0xBB;
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Wait until packet is processed */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(100);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Ensure entry was not added */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if ((ARP_RESOLVED == arptab[i].state)
                && (netaddrequal(&praddr, &arptab[i].praddr)))
            {
                entry = &arptab[i];
                break;
            }
        }
        failif((entry != NULL), "Entry inserted");
    }

    /* Test receive reply for non-supported protocol type */
    testPrint(verbose, "Receive ARP reply, bad protocol type");
    /* Get 7th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Wait until packet is processed */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(100);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Ensure entry was not added */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if ((ARP_RESOLVED == arptab[i].state)
                && (netaddrequal(&praddr, &arptab[i].praddr)))
            {
                entry = &arptab[i];
                break;
            }
        }
        failif((entry != NULL), "Entry inserted");
    }

    /* Test receive reply for existing resolved ARP table entry */
    testPrint(verbose, "Receive ARP reply, resolved entry");
    /* Get 8th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    praddr.addr[3] = 3;
    hwaddr.addr[5] = 0x0C;
    nout = pelp->nout;
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Wait until packet is processed */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(100);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Check entry */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if ((ARP_RESOLVED == arptab[i].state)
                && (netaddrequal(&praddr, &arptab[i].praddr)))
            {
                entry = &arptab[i];
                break;
            }
        }
        failif((FALSE == netaddrequal(&hwaddr, &entry->hwaddr)),
               "Entry incorrect");
    }

    /* Test receive request not for me */
    testPrint(verbose, "Receive ARP request, not mine");
    /* Get 9th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    praddr.addr[3] = 4;
    hwaddr.addr[5] = 0xDD;
    nproc = netptr->nproc;
    write(ELOOP, data, phdr.caplen);
    /* Wait until packet is processed */
    wait = 0;
    while ((wait < MAX_WAIT) && (netptr->nproc == nproc))
    {
        wait++;
        sleep(100);
    }
    if (MAX_WAIT == wait)
    {
        failif(TRUE, "Wait time expired");
    }
    else
    {
        /* Ensure entry was not added */
        entry = NULL;
        for (i = 0; i < ARP_NENTRY; i++)
        {
            if ((ARP_RESOLVED == arptab[i].state)
                && (netaddrequal(&praddr, &arptab[i].praddr)))
            {
                entry = &arptab[i];
                break;
            }
        }
        failif((entry != NULL), "Entry inserted");
    }

    /* Test arpSendRequest */
    testPrint(verbose, "Send ARP request (bad params)");
    failif((SYSERR != arpSendRqst(NULL)), "");

    /* Test arpSendRequest */
    testPrint(verbose, "Send ARP request");
    /* Get 10th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    entry = &arptab[0];
    entry->state = ARP_UNRESOLVED;
    entry->nif = netptr;
    praddr.addr[3] = 2;
    hwaddr.addr[5] = 0xBB;
    netaddrcpy(&entry->hwaddr, &hwaddr);
    netaddrcpy(&entry->praddr, &praddr);
    entry->expires = clktime + ARP_TTL_UNRESOLVED;
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    if (SYSERR == arpSendRqst(entry))
    {
        failif(TRUE, "Returned SYSERR");
    }
    else
    {
        control(ELOOP, ELOOP_CTRL_GETHOLD, (int)buf, ELOOP_BUFSIZE);
        failif((memcmp(buf, data, phdr.caplen) != 0), "");
    }

    /* Test arpLookup */
    testPrint(verbose, "Lookup address (bad params)");
    failif((SYSERR != arpLookup(NULL, NULL, NULL)), "");

    /* Test arpLookup */
    testPrint(verbose, "Lookup existing resolved address");
    for (i = 0; i < ARP_NENTRY; i++)
    {
        arpFree(&arptab[i]);
    }
    entry = &arptab[0];
    entry->state = ARP_RESOLVED;
    entry->nif = netptr;
    praddr.addr[3] = 1;
    hwaddr.addr[5] = 0xAA;
    netaddrcpy(&entry->hwaddr, &hwaddr);
    netaddrcpy(&entry->praddr, &praddr);
    entry->expires = clktime + ARP_TTL_UNRESOLVED;
    i = arpLookup(netptr, &praddr, &addrbuf);
    if ((SYSERR == i) || (TIMEOUT == i))
    {
        failif(TRUE, "Returned SYSERR or TIMEOUT");
    }
    else
    {
        failif((FALSE == netaddrequal(&addrbuf, &hwaddr)),
               "Wrong address");
    }

    /* Test arpLookup */
    testPrint(verbose, "Lookup existing unresolved address");
    entry = &arptab[1];
    entry->state = ARP_UNRESOLVED;
    entry->nif = netptr;
    praddr.addr[3] = 2;
    hwaddr.addr[5] = 0xBB;
    netaddrcpy(&entry->hwaddr, &hwaddr);
    netaddrcpy(&entry->praddr, &praddr);
    entry->expires = clktime + ARP_TTL_UNRESOLVED;
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    request = data;
    wait = phdr.caplen;
    /* Get 11th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    tid =
        ready(create
              ((void *)lookupTest, INITSTK, INITPRIO, "lookupTest", 4,
               request, wait, data, phdr.caplen), RESCHED_NO);
    i = arpLookup(netptr, &praddr, &addrbuf);
    if ((SYSERR == i) || (TIMEOUT == i))
    {
        kill(tid);
        failif(TRUE, "Returned SYSERR or TIMEOUT");
    }
    else
    {
        failif((FALSE == netaddrequal(&addrbuf, &hwaddr)),
               "Wrong address");
    }

    /* Test arpLookup */
    testPrint(verbose, "Lookup max threads wait for resolve");
    entry->state = ARP_UNRESOLVED;
    entry->count = ARP_NTHRWAIT;
    failif((arpLookup(netptr, &praddr, &addrbuf) != SYSERR), "");

    /* Test arpLookup */
    testPrint(verbose, "Lookup non-existing unresolved addr");
    praddr.addr[3] = 3;
    hwaddr.addr[5] = 0xCC;
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    request = data;
    wait = phdr.caplen;
    /* Get 12th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    request = data;
    wait = phdr.caplen;
    /* Get 13th packet */
    data += phdr.caplen;
    memcpy(&phdr, data, sizeof(phdr));
    data += sizeof(phdr);
    if (PCAP_MAGIC != pcap.magic)
    {
        phdr.caplen = endswap(phdr.caplen);
    }
    tid =
        ready(create
              ((void *)lookupTest, INITSTK, INITPRIO, "lookupTest", 4,
               request, wait, data, phdr.caplen), RESCHED_NO);
    i = arpLookup(netptr, &praddr, &addrbuf);
    if ((SYSERR == i) || (TIMEOUT == i))
    {
        kill(tid);
        failif(TRUE, "Returned SYSERR or TIMEOUT");
    }
    else
    {
        failif((FALSE == netaddrequal(&addrbuf, &hwaddr)),
               "Wrong address");
    }

    /* Test arpLookup */
    testPrint(verbose, "Lookup timeout");
    praddr.addr[3] = 4;
    hwaddr.addr[5] = 0xDD;
    control(ELOOP, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_DROPALL, NULL);
    failif((SYSERR != arpLookup(netptr, &praddr, &addrbuf)), "");

    /* Stop loopback ethernet and network interface */
    testPrint(verbose, "Test case cleanup");
    for (i = 0; i < ARP_NENTRY; i++)
    {
        arpFree(&arptab[i]);
    }
    failif(((SYSERR == netFreebuf(pkt)) || (SYSERR == netDown(ELOOP))
            || (SYSERR == close(ELOOP))), "");

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
