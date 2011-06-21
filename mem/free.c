/**
 * @file free.c
 * @provides free.
 * Free memory from a user thread.
 *
 * $Id: free.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>
#include <safemem.h>
#include <interrupt.h>
#include <thread.h>

/**
 * Attempt to free block of memory based on malloc() accounting
 * information stored in preceding two words.
 * @param *pmem pointer to memory block
 */
void free(void *base)
{
#ifdef UHEAP_SIZE
    struct thrent *thread;
    struct memblock *block, *next, *prev;
    irqmask im;
    uint top;


    /* memptr points at the memblock to free */
    block = (struct memblock *)base;

    /* back up to accounting information */
    block--;

    /* make basic block checks */
    if (block->next != block)
    {
        return;
    }

    /* unmap pages from system page table */
    safeUnmapRange(block, block->length);

    im = disable();

    /* get pointer to current thread */
    thread = &thrtab[thrcurrent];

    prev = &(thread->memlist);
    next = thread->memlist.next;
    while ((next != NULL) && (next < block))
    {
        prev = next;
        next = next->next;
    }

    /* find top of previous memblock */
    if (prev == &(thread->memlist))
    {
        top = NULL;
    }
    else
    {
        top = (ulong)prev + prev->length;
    }

    /* make sure block is not overlapping on prev or next blocks */
    if ((top > (ulong)block)
        || ((next != NULL)
            && ((ulong)block + block->length) > (ulong)next))
    {
        restore(im);
        return;
    }

    thread->memlist.length += block->length;

    /* coalesce with previous block if adjacent */
    if (top == (ulong)block)
    {
        prev->length += block->length;
        block = prev;
    }
    else
    {
        block->next = next;
        prev->next = block;
    }

    /* coalesce with next block if adjacent */
    if (((ulong)block + block->length) == (ulong)next)
    {
        block->length += next->length;
        block->next = next->next;
    }

    restore(im);
#endif                          /* UHEAP_SIZE */
}
