/**
 * @file ready.c
 * @provides ready.
 *
 * $Id: ready.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <queue.h>

/**
 * Make a thread eligible for CPU service.
 * @param tid target thread
 * @param resch if RESCHED_YES, reschedules
 * @return OK if thread has been added to readylist, else SYSERR
 */
int ready(tid_typ tid, bool resch)
{
    register struct thrent *thrptr;

    if (isbadtid(tid))
    {
        return SYSERR;
    }

    thrptr = &thrtab[tid];
    thrptr->state = THRREADY;

    insert(tid, readylist, thrptr->prio);

    if (resch == RESCHED_YES)
    {
        resched();
    }
    return OK;
}
