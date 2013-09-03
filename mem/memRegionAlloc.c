/**
 * @file     memRegionAlloc.c
 * User/kernel agnostic memory allocator.  Simply provides a pointer to
 * memory that belongs to the current thread.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>
#include <thread.h>
#include <stdlib.h>

/**
 * Attempt to allocate a region of at least nbytes to the current thread.
 * @param nbytes number of byte to allocate
 * @return pointer to allocated memory
 */
struct memregion *memRegionAlloc(uint nbytes)
{
    struct memregion *region, *remainder;

    /* round to multiple of page size   */
    nbytes = (ulong)roundpage(nbytes);

    /* find region large enough for nbytes */
    region = regfreelist;
    while ((int)region != SYSERR)
    {
        if (region->length > nbytes)
        {
            break;
        }

        /* iterate */
        region = region->next;
    }

    /* we failed to find a region */
    if (SYSERR == (int)region)
    {
        return (struct memregion *)SYSERR;
    }

    /* remove region from regfree list */
    memRegionRemove(region, &regfreelist);

    /* split of remainder (if needed) */
    if (region->length > nbytes)
    {
        remainder = memRegionSplit(region, nbytes);

        if (NULL != (int)remainder)
        {
            /* return remainder to regfree list */
            memRegionInsert(remainder, &regfreelist);
        }
    }

    /* put region in allocated list */
    memRegionInsert(region, &regalloclist);

    /* Allocate region to the current thread */
    region->thread_id = gettid();

    return region;
}
