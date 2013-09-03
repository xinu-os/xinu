/**
 * @file memRegionTransfer.c
 * Transfer a memory region to a new thread.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Transfer a memory region from the owning thread to a new thread.
 * @param start beginning address of memory region.
 * @param tid thread id to transfer region to.
 */
void memRegionTransfer(void *start, tid_typ tid)
{
    struct memregion *region;

    region = memRegionValid(start);
    if (SYSERR == (int)region)
    {
        return;
    }

    region->thread_id = tid;
}
