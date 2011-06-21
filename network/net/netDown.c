/**
 * @file netDown.c
 * @provides netDown
 *
 * $Id: netDown.c 2070 2009-09-18 22:36:02Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <route.h>
#include <stdlib.h>
#include <thread.h>

#ifndef NNETIF
#define NNETIF (-1)
#endif

/**
 * Stop a network interface.
 * @param descrp underlying device
 * @return OK if stopped properly, otherwise SYSERR
 */
syscall netDown(int descrp)
{
    struct netif *netptr;
    tid_typ tid;
    int i = 0;
    irqmask im;

    /* Error check arguments */
    if (isbaddev(descrp))
    {
        return SYSERR;
    }

    /* Determine which network interface is running on underlying device */
    for (i = 0; i < NNETIF; i++)
    {
        if ((NET_ALLOC == netiftab[i].state)
            && (descrp == netiftab[i].dev))
        {
            break;
        }
    }

    /* Return SYSERR if no network interface is opened on device */
    if (i >= NNETIF)
    {
        return SYSERR;
    }

    netptr = &netiftab[i];
    NET_TRACE("Stopping netif %d on device %d", i, descrp);

    im = disable();

    /* Kill receiver threads */
    for (i = 0; i < NET_NTHR; i++)
    {
        tid = netptr->recvthr[i];
        if (isbadtid(tid))
        {
            continue;
        }
        kill(tid);
        netptr->recvthr[i] = BADTID;
    }


    /* Clear all entries in the route table for this network interface */
    if (SYSERR == rtClear(netptr))
    {
        return SYSERR;
    }

    /* Clear and make the network interface free */
    bzero(&netiftab[i], sizeof(struct netif));
    netptr->state = NET_FREE;

    restore(im);

    return OK;
}
