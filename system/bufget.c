/**
 * @file bufget.c
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
 * Allocate a buffer from a buffer pool.  If no buffers are currently available,
 * this function wait until one is, usually rescheduling the thread.  The
 * returned buffer must be freed with buffree() when the calling code is
 * finished with it.
 *
 * @param poolid
 *      Identifier of the buffer pool, as returned by bfpalloc().
 *
 * @return
 *      If @p poolid does not specify a valid buffer pool, returns ::SYSERR;
 *      otherwise returns a pointer to the resulting buffer.
 */
void *bufget(int poolid)
{
    struct bfpentry *bfpptr;
    struct poolbuf *bufptr;
    irqmask im;

    if (isbadpool(poolid))
    {
        return (void *)SYSERR;
    }

    bfpptr = &bfptab[poolid];

    im = disable();
    wait(bfpptr->freebuf);
    bufptr = bfpptr->next;
    bfpptr->next = bufptr->next;
    restore(im);

    bufptr->next = bufptr;
    return (void *)(bufptr + 1);        /* +1 to skip past accounting structure */
}
