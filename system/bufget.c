/**
 * @file bufget.c
 * @provides bufget.
 *
 * $Id: bufget.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <semaphore.h>
#include <interrupt.h>
#include <bufpool.h>

/**
 * Acquire buffer from initialized pool
 * @param poolid index of pool in bptab
 * @return pointer to buffer on success, SYSERR on failure
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
