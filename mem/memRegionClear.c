/**
 * @file memRegionClear.c
 * Clear (initialize) a memory region to safe default values.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Clear all the values of a given memory region to "safe" defaults.
 * @param region region of memory to clear.
 */
void memRegionClear(struct memregion *region)
{
    region->prev = (struct memregion *)SYSERR;
    region->next = (struct memregion *)SYSERR;
    region->start = NULL;
    region->length = 0;
    region->thread_id = 0;
}
