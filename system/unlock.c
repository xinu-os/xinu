/**
 * @file unlock.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <semaphore.h>
#include <monitor.h>

/**
 * @ingroup monitors
 *
 * unlock a monitor. If monitor's count is 0, then free the monitor
 * and signal the monitor's semaphore.
 * @param mon  target monitor
 * @return OK on success, SYSERR on failure
 */
syscall unlock(monitor mon)
{
    register struct monent *monptr;
    irqmask im;

    im = disable();
    if (isbadmon(mon))
    {
        restore(im);
        return SYSERR;
    }

    monptr = &montab[mon];

    /* safety check: this unlock call does not have an associated lock call */
    if (monptr->count <= 0)
    {
        restore(im);
        return SYSERR;
    }

    /* decrement the monitor's count signifying one "unlock" */
    (monptr->count)--;

    /* if this is the top-level unlock call, then free this monitor's lock */
    if (monptr->count == 0)
    {
        monptr->owner = NOOWNER;
        signal(monptr->sem);
    }

    restore(im);
    return OK;
}
