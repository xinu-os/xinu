/**
 * @file stkget.c
 * @provides stkget.
 *
 * $Id: stkget.c 2074 2009-09-21 23:37:28Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>
#include <interrupt.h>
#include <platform.h>

/**
 * Allocate stack memory, returning address of topmost word.
 * @param nbytes bytes of memory to allocate
 * @return address of the topmost word
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
