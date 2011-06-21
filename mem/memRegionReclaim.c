/**
 * @file     memRegionReclaim.c
 * @provides memRegionReclaim
 * Attempt to reclaim an allocated memory region.
 *
 * $Id: memRegionReclaim.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Given a thread id, attempt to reclaim all the memory regions that were
 * allocated to that thread and return them to the region free list.
 * @param tid thread id holding memory regions.
 */
void memRegionReclaim(tid_typ tid)
{
    struct memregion *region, *nextregion;

    /* Free allocated regions with tid */
    region = regalloclist;
    while ((int)region != SYSERR)
    {
        /* store next region before mutating */
        nextregion = region->next;

        if (region->thread_id == tid)
        {
            memRegionRemove(region, &regalloclist);
            safeUnmapRange(region->start, region->length);
            memRegionInsert(region, &regfreelist);
        }

        region = nextregion;
    }
}
