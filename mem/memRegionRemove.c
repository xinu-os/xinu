/**
 * @file memRegionRemove.c
 * Remove a memory region from a region list.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Remove an existing region from a list.
 * @param region Region to remove.
 * @param list List the region exists in.
 */
void memRegionRemove(struct memregion *region, struct memregion **list)
{
    /* If region has previous, set prev->next to region->next */
    if ((int)(region->prev) != SYSERR)
    {
        region->prev->next = region->next;
    }
    else
    {
        /* otherwise, update the list pointer */
        *list = region->next;
    }

    /* If region has next, set next->prev to region->prev */
    if ((int)(region->next) != SYSERR)
    {
        region->next->prev = region->prev;
    }

    /* clear prev/next fields of this region */
    region->prev = (struct memregion *)SYSERR;
    region->next = (struct memregion *)SYSERR;
}
