/**
 * @file bfpfree.c
 * @provides bfpfree.
 *
 * $Id: bfpfree.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <memory.h>
#include <bufpool.h>

syscall bfpfree(int id)
{
    struct bfpentry *bfpptr;
    irqmask im;

    if (isbadpool(id))
    {
        return SYSERR;
    }

    bfpptr = &bfptab[id];

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
