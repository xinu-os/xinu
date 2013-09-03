/**
 * @file bfpalloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <memory.h>
#include <bufpool.h>

/**
 * @ingroup memory_mgmt
 *
 * Acquire heap storage and subdivide into buffers.
 *
 * @param bufsize
 *      Size of individual buffers, in bytes.
 *
 * @param nbuf
 *      Number of buffers in the pool.
 *
 * @return
 *      On success, returns an identifier for the buffer pool that can be passed
 *      to bufget() or bfpfree().  On failure, returns ::SYSERR.
 */
int bfpalloc(uint bufsize, uint nbuf)
{
    struct bfpentry *bfpptr;
    struct poolbuf *bufptr;
    int id, buffer;
    irqmask im;

    bufsize = roundword(bufsize) + sizeof(struct poolbuf);

    if (bufsize > POOL_MAX_BUFSIZE ||
        bufsize < POOL_MIN_BUFSIZE || nbuf > POOL_MAX_NBUFS || nbuf < 1)
    {
        return SYSERR;
    }

    im = disable();
    for (id = 0; id < NPOOL; id++)
    {
        bfpptr = &bfptab[id];
        if (BFPFREE == bfpptr->state)
        {
            break;
        }
    }
    if (NPOOL == id)
    {
        restore(im);
        return SYSERR;
    }
    bfpptr->state = BFPUSED;
    restore(im);

    bfpptr->freebuf = semcreate(0);
    if (SYSERR == (int)bfpptr->freebuf)
    {
        bfpptr->state = BFPFREE;
        return SYSERR;
    }

    bfpptr->nbuf = nbuf;
    bfpptr->bufsize = bufsize;
    bufptr = (struct poolbuf *)memget(nbuf * bufsize);
    if ((void *)SYSERR == bufptr)
    {
        semfree(bfpptr->freebuf);
        bfpptr->state = BFPFREE;
        return SYSERR;
    }
    bfpptr->next = bufptr;
    bfpptr->head = bufptr;
    for (buffer = 0; buffer < nbuf; buffer++)
    {
        bufptr->poolid = id;
        bufptr->next = (struct poolbuf *)((ulong)bufptr + bufsize);
        bufptr = bufptr->next;
    }
    signaln(bfpptr->freebuf, nbuf);

    return id;
}
