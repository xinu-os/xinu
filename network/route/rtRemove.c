/**
 * @file rtRemove.c
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
 * Removes any entry based on its destination.
 * @param dst destination IP address
 * @return OK if the entry is removed successfully, otherwise SYSERR
 */
syscall rtRemove(const struct netaddr *dst)
{
    int i;
    irqmask im;

    /* Error check pointers */
    if (NULL == dst)
    {
        return SYSERR;
    }

    im = disable();
    for (i = 0; i < RT_NENTRY; i++)
    {
        if ((RT_USED == rttab[i].state)
            && netaddrequal(dst, &rttab[i].dst))
        {
            rttab[i].state = RT_FREE;
            rttab[i].nif = NULL;
        }
    }
    restore(im);
    return OK;
}
