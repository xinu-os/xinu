/**
 * @file safeUnmap.c
 * @provides safeUnmap.
 * Unmap a page from the page table.
 *
 * $Id: safeUnmap.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>
#include <mips.h>
#include <stdlib.h>

/**
 * Unmap a page from the page table.
 * @param page page to remove from page table.
 * @return non-zero value on failure.
 */
int safeUnmap(void *page)
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

    /* make sure entry is correctly mapped */
    if ((entry->entry & 0xffffffc0) != ((uint)page >> 6))
    {
        return 1;               /* corrupted page */
    }

    /* clear entry from page table */
    bzero(entry, sizeof(struct pgtblent));

    return 0;
}
