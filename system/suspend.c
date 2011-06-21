/**
 * @file suspend.c
 * @provides suspend.
 *
 * $Id: suspend.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <queue.h>

/**
 * Suspend a thread, placing it in hibernation
 * @param tid target thread
 * @return priority or SYSERR
 */
syscall suspend(tid_typ tid)
{
    register struct thrent *thrptr;     /* thread control block  */
    irqmask im;
    int prio;

    im = disable();
    if (isbadtid(tid) || (NULLTHREAD == tid))
    {
        restore(im);
        return SYSERR;
    }
    thrptr = &thrtab[tid];
    if ((thrptr->state != THRCURR) && (thrptr->state != THRREADY))
    {
        restore(im);
        return SYSERR;
    }
    if (THRREADY == thrptr->state)
    {
        getitem(tid);           /* removes from queue */
        thrptr->state = THRSUSP;
    }
    else
    {
        thrptr->state = THRSUSP;
        resched();
    }
    prio = thrptr->prio;
    restore(im);
    return prio;
}
