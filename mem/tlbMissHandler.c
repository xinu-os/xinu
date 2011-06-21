/**
 * @file tlbMissHandler.c
 * @provides tlbMissHandler.
 * TLB handler written in C.  Should remain unused, but presents a higher
 * level view of what the TLB handler does.
 *
 * $Id: tlbMissHandler.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <safemem.h>
#include <mips.h>
#include <thread.h>
#include <stdio.h>
#include <tlb.h>

/**
 * Slower (C based) TLB handler, though provides an easier way to
 * understand.
 * @param vpn_fault faulting address
 */
void tlbMissHandler(int vpn_fault, int *entrylo)
{
    int index, align_index;
    struct pgtblent *entry;

    /* determine which page of page tables entry is in */
    index = (((uint)vpn_fault & PMEM_MASK) >> 12);
    align_index = index & ~(0x01);
    entry = &(pgtbl[align_index]);

    index = index & 0x01;
    /* make sure current address space is mapped address space */
    if (entry[index].entry == NULL || entry[index].asid != thrcurrent)
    {
        /* no mapping */
        fprintf(stderr, "Memory protection violation (0x%08x).\n",
                vpn_fault);
        exlreset();
        kill(thrcurrent);
        while (1)
        {
            /* violation means thread cannot proceed */
        }
    }

    entrylo[0] = entry[0].entry | 0x10;
    entrylo[1] = entry[1].entry | 0x10;
}
