/**
 * @file memRegionTransfer.c
 * @provides memRegionTransfer.
 * Transfer a memory region to a new thread.
 *
 * $Id: memRegionTransfer.c 2020 2009-08-13 17:50:08Z mschul $
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
