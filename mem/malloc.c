/**
 * @file malloc.c
 * Allocate memory to a user thread.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>
#include <safemem.h>
#include <interrupt.h>
#include <thread.h>
#include <stdlib.h>

/**
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *malloc(uint nbytes)
{
#ifdef UHEAP_SIZE
    irqmask im;
    struct thrent *thread;
    struct memregion *region;
    struct memblock *prev, *curr, *leftover;

    /* we don't allocate 0 bytes. */
    if (0 == nbytes)
    {
        return NULL;
    }

    /* round request to size of memblock */
    nbytes = (uint)roundmb(nbytes);

    /* make room for accounting information */
    nbytes += sizeof(struct memblock);

    /* setup thread pointer */
    thread = &thrtab[thrcurrent];

    im = disable();

    prev = &(thread->memlist);
    curr = thread->memlist.next;
    while (curr != NULL)
    {
        if (curr->length == nbytes)
        {
            prev->next = curr->next;
            thread->memlist.length -= nbytes;

            break;
        }
        else if (curr->length > nbytes)
        {
            /* split block into two */
            leftover = (struct memblock *)((ulong)curr + nbytes);
            prev->next = leftover;
            leftover->next = curr->next;
            leftover->length = curr->length - nbytes;
            thread->memlist.length -= nbytes;

            break;
        }
        prev = curr;
        curr = curr->next;
    }

    /* did we allocate thread memory? */
    if (curr != NULL)
    {
        /* set up accouting info */
        curr->next = curr;
        curr->length = nbytes;

        restore(im);
        return (void *)(curr + 1);
    }

    /* attempt acquire memory from kernel */
    region = memRegionAlloc(nbytes);
    if (SYSERR == (int)region)
    {
        restore(im);
        return NULL;
    }

    /* setup curr pointer */
    curr = region->start;

    /* if we got more memory than needed, break off remaining memory */
    if (region->length > nbytes)
    {
        leftover = (struct memblock *)((uint)region->start + nbytes);
        leftover->next = leftover;
        leftover->length = region->length - nbytes;

        /* place extra memory in freelist */
        free(++leftover);
    }

    /* set accounting info */
    curr->next = curr;
    curr->length = nbytes;

    /* map memory to system page table */
    safeMapRange(curr, nbytes, ENT_USER);

    restore(im);
    return (void *)(curr + 1);
#else
    return NULL;
#endif                          /* UHEAP_SIZE */
}
