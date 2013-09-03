/**
 * @file memRegionInsert.c
 * Insert a memory region into a list of memory regions (free or
 * allocated).
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

extern struct memregion *regalloclist, *regfreelist;
static void memRegionCoalesce(struct memregion *region);

/**
 * Insert a region into a region list.  This will coalesce with neighboring
 * regions if applicable (list is free list).
 * @param region Region to insert.
 * @param list List to insert region in.
 */
void memRegionInsert(struct memregion *region, struct memregion **list)
{
    struct memregion *current, *previous;

    /* See if list has *any* regions, if not just add. */
    if (SYSERR == (int)(*list))
    {
        *list = region;

        return;
    }

    /* Look for existing region with address larger than our region */
    current = *list;
    while ((SYSERR != (int)current) && (current->start < region->start))
    {
        /* Iterate */
        previous = current;
        current = current->next;
    }

    if (SYSERR == (int)current)
    {
        /* add after current */
        previous->next = region;
        region->prev = previous;
        region->next = (struct memregion *)SYSERR;
    }
    else
    {
        /* add before current */
        region->prev = current->prev;
        region->next = current;
        if ((int)(current->prev) != SYSERR)
        {
            current->prev->next = region;
        }
        else
        {
            *list = region;
        }
        current->prev = region;
    }

    /* region in free list, coalesce */
    if (*list == regfreelist)
    {
        memRegionCoalesce(region);
    }
}

/**
 * Coalesce adjacent memory regions into one larger region.  This allows
 * large regions to be allocated if enough contiguous space exists.
 * @param region pointer to region to attempt to coalesce
 */
static void memRegionCoalesce(struct memregion *region)
{
    struct memregion *adjacent;

    /* does region bump with region->next? */
    adjacent = region->next;
    if (((int)adjacent != SYSERR)
        && ((uint)(region->start) + region->length)
        == (uint)(adjacent->start))
    {
        /* coalesce region with region->next */
        region->length += adjacent->length;
        region->next = adjacent->next;
        if (SYSERR != (int)adjacent->next)
        {
            adjacent->next->prev = region;
        }
    }

    /* does previous region bump with region? */
    adjacent = region->prev;
    if (((int)adjacent != SYSERR)
        && ((uint)(adjacent->start) + adjacent->length ==
            (uint)(region->start)))
    {
        /* coalesce region with region->prev */
        adjacent->length += region->length;
        adjacent->next = region->next;
        if (SYSERR != (int)region->next)
        {
            region->next->prev = adjacent;
        }
    }
}
