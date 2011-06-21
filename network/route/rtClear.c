/**
 * @file rtClear.c
 * @provides rtClear.
 * 
 * $Id: rtClear.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>

/**
 * Removes all entries for a network interface from the routing table. 
 * @param nif network interface 
 * @return OK if entries are removed successfully, otherwise SYSERR
 */
syscall rtClear(struct netif *nif)
{
    int i;
    irqmask im;

    /* Error check pointers */
    if (NULL == nif)
    {
        return SYSERR;
    }

    im = disable();
    for (i = 0; i < RT_NENTRY; i++)
    {
        if ((RT_USED == rttab[i].state) && (nif == rttab[i].nif))
        {
            rttab[i].state = RT_FREE;
            rttab[i].nif = NULL;
        }
    }
    restore(im);
    return OK;
}
