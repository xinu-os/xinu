/**
 * @file safeInit
 * @provides safeInit.
 * Initialize the memory protection subsystem.
 *
 * $Id: safeInit.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>

#include <memory.h>
#include <safemem.h>
#include <platform.h>
#include <stdlib.h>
#include <mips.h>

struct pgtblent *pgtbl = NULL;      /**< system page table address */
uint pgtbl_nents = 0;               /**< number of entries in page table */

/**
 * Initialize the memory protection subsystem.  Allocates needed memory and
 * configures initial values for page tables.
 */
void safeInit(void)
{
    uint ents_per_page;
    uint nbytes;

    /* number of pages in memory */
    pgtbl_nents =
        roundpage((uint)platform.maxaddr & PMEM_MASK) / PAGE_SIZE;

    /* number of entries that can fit on a single page */
    ents_per_page = PAGE_SIZE / sizeof(struct pgtblent);

    /* page table size (in bytes) */
    nbytes = (pgtbl_nents / ents_per_page) * PAGE_SIZE;

    /* allocate memory system page table */
    pgtbl = (struct pgtblent *)memget(nbytes);

    /* clear page table */
    bzero(pgtbl, nbytes);
}
