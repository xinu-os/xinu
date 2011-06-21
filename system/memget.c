/**
 * @file memget.c
 * @provides memget.
 *
 * $Id: memget.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <interrupt.h>
#include <memory.h>

/**
 * Allocate heap storage, returning lowest word address
 * @param nbytes  bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *memget(uint nbytes)
{
    register struct memblock *prev, *curr, *leftover;
    irqmask im;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

    prev = &memlist;
    curr = memlist.next;
    while (curr != NULL)
    {
        if (curr->length == nbytes)
        {
            prev->next = curr->next;
            memlist.length -= nbytes;

            restore(im);
            return (void *)(curr);
        }
        else if (curr->length > nbytes)
        {
            /* split block into two */
            leftover = (struct memblock *)((ulong)curr + nbytes);
            prev->next = leftover;
            leftover->next = curr->next;
            leftover->length = curr->length - nbytes;
            memlist.length -= nbytes;

            restore(im);
            return (void *)(curr);
        }
        prev = curr;
        curr = curr->next;
    }
    restore(im);
    return (void *)SYSERR;
}
