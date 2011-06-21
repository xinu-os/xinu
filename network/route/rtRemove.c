/**
 * @file rtRemove.c
 * @provides rtRemove
 * 
 * $Id: rtRemove.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>

/**
 * Removes any entry based on its destination.
 * @param dst destination IP address
 * @return OK if the entry is removed successfully, otherwise SYSERR
 */
syscall rtRemove(struct netaddr *dst)
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
