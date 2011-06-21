/**
 * @file resched.c
 * @provides resched.
 *
 * $Id: resched.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <clock.h>
#include <queue.h>
#include <memory.h>

extern void ctxsw(void *, void *);
int resdefer;                   /* >0 if rescheduling deferred */

/**
 * Reschedule processor to highest priority ready thread.
 * Upon entry, thrcurrent gives current thread id.
 * Threadtab[thrcurrent].pstate gives correct NEXT state
 * for current thread if other than THRREADY.
 * @return OK when the thread is context switched back
 */
int resched(void)
{
    uchar asid;                 /* address space identifier */
    struct thrent *throld;      /* old thread entry */
    struct thrent *thrnew;      /* new thread entry */

    if (resdefer > 0)
    {                           /* if deferred, increase count & return */
        resdefer++;
        return (OK);
    }

    throld = &thrtab[thrcurrent];

    throld->intmask = disable();

    if (THRCURR == throld->state)
    {
        if (nonempty(readylist) && (throld->prio > firstkey(readylist)))
        {
            restore(throld->intmask);
            return OK;
        }
        throld->state = THRREADY;
        insert(thrcurrent, readylist, throld->prio);
    }

    /* get highest priority thread from ready list */
    thrcurrent = dequeue(readylist);
    thrnew = &thrtab[thrcurrent];
    thrnew->state = THRCURR;

    /* change address space identifier to thread id */
    asid = thrcurrent & 0xff;
  asm("mtc0 %0, $10": :"r"(asid));

    ctxsw(&throld->stkptr, &thrnew->stkptr);

    /* old thread returns here when resumed */
    restore(throld->intmask);
    return OK;
}
