/* @file netInit.c
 * @provides netInit
 *
 * $Id: netInit.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <icmp.h>
#include <bufpool.h>
#include <network.h>
#include <route.h>
#include <stdlib.h>
#include <stdio.h>
#include <tcp.h>

#ifndef NNETIF
#define NNETIF 0
#endif

struct netif netiftab[NNETIF];
int netpool;

/**
 * Initialize network interfaces
 * @return OK if initialized properly, otherwise SYSERR
 */
syscall netInit()
{
    int i = 0;

    /* Clear each network interface */
    for (i = 0; i < NNETIF; i++)
    {
        bzero(&netiftab[i], sizeof(struct netif));
        netiftab[i].state = NET_FREE;
    }

    /* Allocate packet buffer pool */
    netpool = bfpalloc(NET_MAX_PKTLEN + sizeof(struct packet),
                       NET_POOLSIZE);
    NET_TRACE("netpool has been assigned pool ID %d.\r\n", netpool);
    if (SYSERR == netpool)
    {
        return SYSERR;
    }

    /* Initialize ARP */
    if (SYSERR == arpInit())
    {
        return SYSERR;
    }

    if (SYSERR == rtInit())
    {
        return SYSERR;
    }

    /* Initialize ICMP */
    if (SYSERR == icmpInit())
    {
        return SYSERR;
    }

    /* Initialize TCP */
#if NTCP
    i = create((void *)tcpTimer, INITSTK, INITPRIO, "tcpTimer", 0);
    if (SYSERR == i)
    {
        return SYSERR;
    }
    else
    {
        ready(i, RESCHED_NO);
    }
#endif                          /* NTCP */

    return OK;
}
