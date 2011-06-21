/**
 * @file bfpalloc.c
 * @provides bfpalloc.
 *
 * $Id: bfpalloc.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <memory.h>
#include <bufpool.h>

/**
 * Acquire heap storage and subdivide into buffers
 * @param bufsize size of individual buffers in bytes
 * @param nbuf count of bufsize buffers in pool
 * @return index into bfptab of initialized pool, SYSERR on failure
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
    bfpptr->freebuf = semcreate(0);
    if (SYSERR == (int)bfpptr->freebuf)
    {
        restore(im);
        return SYSERR;
    }
    restore(im);

    bfpptr->nbuf = nbuf;
    bfpptr->bufsize = bufsize;
    bufptr = (struct poolbuf *)memget(nbuf * bufsize);
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
