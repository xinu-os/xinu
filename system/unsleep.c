/**
 * @file unsleep.c
 * @provides unsleep.
 *
 * $Id: unsleep.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stddef.h>
#include <interrupt.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

/**
 * Remove thread from the sleep queue prematurely
 * @param tid  target thread
 * @return OK if thread removed, else SYSERR
 */
syscall unsleep(tid_typ tid)
{
    register struct thrent *thrptr;
    irqmask im;
    tid_typ next = 0;

    im = disable();

    if (isbadtid(tid))
    {
        restore(im);
        return SYSERR;
    }

    thrptr = &thrtab[tid];
    if ((thrptr->state != THRSLEEP) && (thrptr->state != THRTMOUT))
    {
        restore(im);
        return SYSERR;
    }

    next = quetab[tid].next;
    if (next < NTHREAD)
    {
        quetab[next].key += quetab[tid].key;
    }

    getitem(tid);
    restore(im);
    return OK;
}
