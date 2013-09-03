/**
 * @file kill.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <queue.h>
#include <memory.h>
#include <safemem.h>

extern void xdone(void);

/**
 * @ingroup threads
 *
 * Kill a thread and remove it from the system
 * @param tid target thread
 * @return OK on success, SYSERR otherwise
 */
syscall kill(tid_typ tid)
{
    register struct thrent *thrptr;     /* thread control block */
    irqmask im;

    im = disable();
    if (isbadtid(tid) || (NULLTHREAD == tid))
    {
        restore(im);
        return SYSERR;
    }
    thrptr = &thrtab[tid];

    if (--thrcount <= 1)
    {
        xdone();
    }

#ifdef UHEAP_SIZE
    /* reclaim used memory regions */
    memRegionReclaim(tid);
#endif                          /* UHEAP_SIZE */

    send(thrptr->parent, tid);

    stkfree(thrptr->stkbase, thrptr->stklen);

    switch (thrptr->state)
    {
    case THRSLEEP:
        unsleep(tid);
        thrptr->state = THRFREE;
        break;
    case THRCURR:
        thrptr->state = THRFREE;        /* suicide */
        resched();

    case THRWAIT:
        semtab[thrptr->sem].count++;

    case THRREADY:
        getitem(tid);           /* removes from queue */

    default:
        thrptr->state = THRFREE;
    }

    restore(im);
    return OK;
}
