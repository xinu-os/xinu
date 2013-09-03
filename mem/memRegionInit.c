/**
 * @file memRegionInit.c
 * Initializes system memory for region allocator.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <mips.h>
#include <memory.h>
#include <safemem.h>

struct memregion *regtab;
struct memregion *regfreelist;
struct memregion *regalloclist;

/**
 * Initialize free memory into page aligned regions.  Takes necessary
 * overhead of region table for the amount of memory on platform.
 * @param memory_start Base address of heap space.
 */
void memRegionInit(void *memory_start, uint memory_size)
{
    int i;
    int max_regions;
    uint table_size;

    /* Initialize free memory list */
    memory_size = truncpage((uint)memory_start + memory_size)
        - (uint)memory_start;
    memory_start = (void *)roundpage(memory_start);

    /* find how many regions we need */
    max_regions = (memory_size / PAGE_SIZE);
    table_size = max_regions * sizeof(struct memregion);

    /* get memory for the region table */
    regtab = (struct memregion *)memget(table_size);

    /* configure region table */
    regtab[0].prev = (struct memregion *)SYSERR;
    regtab[0].next = (struct memregion *)SYSERR;
    regtab[0].start = memory_start;
    regtab[0].length = memory_size;
    regtab[0].thread_id = 0;

    /* clear all other regions */
    for (i = 1; i < max_regions; i++)
    {
        memRegionClear(&(regtab[i]));
    }

    regalloclist = (struct memregion *)SYSERR;
    regfreelist = &(regtab[0]);

#ifdef DETAIL
    kprintf("Allocated %d bytes to user memory heap.\r\n", memory_size);
#endif
}
