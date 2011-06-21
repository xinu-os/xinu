/**
 * @file resume.c
 * @provides resume.
 *
 * $Id: resume.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <queue.h>

/**
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
    thrptr = &thrtab[tid];
    if (isbadtid(tid) || (thrptr->state != THRSUSP))
    {
        restore(im);
        return SYSERR;
    }

    prio = thrptr->prio;
    ready(tid, RESCHED_YES);
    restore(im);
    return prio;
}
