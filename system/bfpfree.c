/**
 * @file bfpfree.c
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
 * Frees the memory allocated for a buffer pool.
 *
 * @param poolid
 *      Identifier of the buffer pool to free, as returned by bfpalloc().
 *
 * @return
 *      ::OK if the buffer pool was valid and was successfully freed; otherwise
 *      ::SYSERR.  If @p poolid specified a valid buffer pool, then this
 *      function can only return ::SYSERR as a result of memory corruption.
 */
syscall bfpfree(int poolid)
{
    struct bfpentry *bfpptr;
    irqmask im;

    if (isbadpool(poolid))
    {
        return SYSERR;
    }

    bfpptr = &bfptab[poolid];

    im = disable();
    bfpptr->state = BFPFREE;
    if (SYSERR == memfree(bfpptr->head, bfpptr->nbuf * bfpptr->bufsize))
    {
        restore(im);
        return SYSERR;
    }
    if (SYSERR == semfree(bfpptr->freebuf))
    {
        restore(im);
        return SYSERR;
    }
    restore(im);

    return OK;
}
