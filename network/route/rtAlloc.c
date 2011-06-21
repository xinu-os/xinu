/**
 * @file rtAlloc.c
 * @provides rtAlloc
 *
 * $Id: rtAlloc.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <route.h>

/* Allocates an entry from the route table.
 * @return entry in route table, NULL if all used, SYSERR if error occurs
 */
struct rtEntry *rtAlloc(void)
{
    int i;
    irqmask im;

    RT_TRACE("Allocating route entry");

    im = disable();
    for (i = 0; i < RT_NENTRY; i++)
    {
        /* If entry is free, return entry */
        if (RT_FREE == rttab[i].state)
        {
            rttab[i].state = RT_PEND;
            RT_TRACE("Free entry %d", i);
            restore(im);
            return &rttab[i];
        }
    }

    restore(im);
    RT_TRACE("No free entry");
    return NULL;
}
