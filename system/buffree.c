/**
 * @file buffree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <semaphore.h>
#include <interrupt.h>
#include <bufpool.h>

/**
 * @ingroup memory_mgmt
 *
 * Return a buffer to its buffer pool.
 *
 * @param buffer
 *      Address of buffer to free, as returned by bufget().
 *
 * @return
 *      ::OK if buffer was successfully freed; otherwise ::SYSERR.  ::SYSERR can
 *      only be returned as a result of memory corruption or passing an invalid
 *      @p buffer argument.
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
