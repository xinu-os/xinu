/**
 * @file buffree.c
 * @provides buffree.
 *
 * $Id: buffree.c 2079 2009-10-05 22:32:48Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <semaphore.h>
#include <interrupt.h>
#include <bufpool.h>

/**
 * Return buffer to pool
 * @param buffer address of returning buffer
 * @return OK on success, SYSERR on failure
 */
syscall buffree(void *buffer)
{
    struct bfpentry *bfpptr;
    struct poolbuf *bufptr;
    irqmask im;

    bufptr = ((struct poolbuf *)buffer) - 1;

    if (isbadpool(bufptr->poolid))
    {
        return SYSERR;
    }

    if (bufptr->next != bufptr)
    {
        return SYSERR;
    }

    bfpptr = &bfptab[bufptr->poolid];

    im = disable();
    bufptr->next = bfpptr->next;
    bfpptr->next = bufptr;
    restore(im);
    signaln(bfpptr->freebuf, 1);

    return OK;
}
