/**
 * @file rtClear.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 *
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
