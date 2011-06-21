/**
 * @file memRegionSplit.c
 * @provides memRegionSplit.
 * Split a region of memory into two parts.
 *
 * $Id: memRegionSplit.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>
#include <mips.h>

static struct memregion *memRegionDiscover(void *address);

/**
 * Given a region, split into two separate chunks. Passed region will be
 * of size length and remainder will be returned.
 * @param region region to split
 * @param length length of region after split
 * @return remainder of original region after split
 */
struct memregion *memRegionSplit(struct memregion *region, uint length)
{
    struct memregion *remainder = NULL;
    void *rem_start;

    /* Make sure length is a page size */
    length = roundpage(length);

    /* Check if region is large enough to split */
    if (region->length < length)
    {
        return (struct memregion *)SYSERR;
    }

    /* Check if region will have leftovers */
    if (region->length == length)
    {
        return (struct memregion *)NULL;
    }

    /* find page table entry for remainder */
    rem_start = (void *)((uint)(region->start) + length);
    remainder = memRegionDiscover(rem_start);

    /* Configure remainder structure with correct values */
    remainder->prev = (struct memregion *)SYSERR;
    remainder->next = (struct memregion *)SYSERR;
    remainder->start = rem_start;
    remainder->length = region->length - length;
    remainder->thread_id = 0;

    /* Update length for updated region structure */
    region->length = length;

    return remainder;
}

/**
 * Given a page address, find the corresponding memory region dedicated
 * to that page.
 * @param address address within memory region
 * @return pointer to the memory region.
 */
static struct memregion *memRegionDiscover(void *address)
{
    uint addr, start;
    int offset;

    /* convert memory address into page starting address */
    addr = (uint)truncpage(address);

    /* ignore the high bits */
    addr = addr & PMEM_MASK;

    /* allocatable regions start at regtab[0] */
    start = (uint)(regtab[0].start) & PMEM_MASK;

    /* calculate index of address in page table */
    offset = ((addr - start) / PAGE_SIZE);

    return &(regtab[offset]);
}
