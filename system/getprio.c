/**
 * @file getprio.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * Return the scheduling priority of a thread
 * @param tid thread ID
 * @return priority of thread on success, SYSERR on failure
 */
syscall getprio(tid_typ tid)
{
    int prio;
    irqmask im;

    im = disable();
    if (isbadtid(tid))
    {
        restore(im);
        return SYSERR;
    }

    prio = thrtab[tid].prio;
    restore(im);
    return prio;
}
