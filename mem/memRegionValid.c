/**
 * @file memRegionValid.c
 * Determine if and address is within an allocated region.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Determine if the address in memory is in an allocated memory region.
 * @param start address of memory to check.
 * @return memory region belonging to start, SYSERR if not found.
 */
struct memregion *memRegionValid(void *start)
{
    struct memregion *region;
    start = (void *)truncpage(start);

    /* check allocated regions */
    region = regalloclist;
    while (((int)region != SYSERR) && (region->start != start))
    {
        region = region->next;
    }

    return region;
}
