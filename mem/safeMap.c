/**
 * @file safeMap.c
 * Map a page of memory to the page table.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>

#include <safemem.h>
#include <mips.h>
#include <thread.h>

/**
 * Map a page of memory to the page table record with attributes.
 * @param page page to insert into page table.
 * @param attr attributes to apply to page.
 * @return non-zero value on failure.
 */
int safeMap(void *page, short attr)
{
    int index;
    struct pgtblent *entry;

    if (NULL == pgtbl || NULL == page)
    {
        return 1;               /* invalid */
    }

    /* find index of page table entry */
    index = (((uint)page & PMEM_MASK) >> 12);
    entry = &(pgtbl[index]);

    /* check if entry is mapped */
    if (entry == (struct pgtblent *)page)
    {
        return 0;               /* already mapped */
    }

    /* check if entry is mapped (to something else) */
    if (entry->entry != NULL)
    {
        return 1;               /* duplicated */
    }

    /* insert entry into page table */
    entry->entry = (uint)page >> 6 | attr;
    entry->asid = gettid();

//    kprintf("Mapped 0x%08x (0x%08x) to pgtbl index %d @ 0x%08x\r\n", page, entry->frame, index, entry);

    return 0;
}
