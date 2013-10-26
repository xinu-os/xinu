/**
 * @file stkget.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>
#include <interrupt.h>
#include <platform.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate stack memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the <b>topmost (highest address)
 *      word</b> of the allocated memory region.  The intention is that this is
 *      the base of a stack growing down.  Free the stack with stkfree() when
 *      done with it.
 */
void *stkget(uint nbytes)
{
    irqmask im;
    struct memblock *prev, *next, *fits, *fitsprev;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (uint)roundmb(nbytes);

    im = disable();

    prev = &memlist;
    next = memlist.next;
    fits = NULL;
    fitsprev = NULL;

    /* scan list for highest block that fits */
    while (next != NULL)
    {
        if (next->length >= nbytes)
        {
            fits = next;
            fitsprev = prev;
        }
        prev = next;
        next = next->next;
    }

    if (NULL == fits)
    {
        /* no block big enough */
        restore(im);
        return (void *)SYSERR;
    }

    if (nbytes == fits->length)
    {
        fitsprev->next = fits->next;
    }
    else
    {
        /* take top portion */
        fits->length -= nbytes;
        fits = (struct memblock *)((ulong)fits + fits->length);
    }

    memlist.length -= nbytes;
    restore(im);
    return (void *)((ulong)fits + nbytes - sizeof(int));
}
