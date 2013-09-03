/**
 * @file chprio.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * Change the scheduling priority of a thread
 * @param tid target thread
 * @param newprio new priority
 * @return old priority of thread
 */
syscall chprio(tid_typ tid, int newprio)
{
    register struct thrent *thrptr;     /* thread control block */
    irqmask im;
    int oldprio;

    im = disable();
    if (isbadtid(tid))
    {
        restore(im);
        return SYSERR;
    }
    thrptr = &thrtab[tid];
    oldprio = thrptr->prio;
    thrptr->prio = newprio;
    restore(im);
    return oldprio;
}
