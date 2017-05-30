/**
 * @file resume.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <queue.h>

/**
 * @ingroup threads
 *
 * Unsuspend a thread, making it ready
 * @param tid target thread
 * @return priority
 */
syscall resume(tid_typ tid)
{
    register struct thrent *thrptr;     /* thread control block  */
    irqmask im;
    int prio;

    im = disable();
    if (isbadtid(tid) || (thrptr->state != THRSUSP))
    {
        restore(im);
        return SYSERR;
    }
    thrptr = &thrtab[tid];

    prio = thrptr->prio;
    ready(tid, RESCHED_YES);
    restore(im);
    return prio;
}
