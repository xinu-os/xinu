/**
 * @file unlock.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Unlock a monitor.
 *
 * The monitor's lock count (indicating the number of times the owning thread
 * has locked the monitor) is decremented.  If the count remains greater than
 * zero, no further action is taken.  If the count reaches zero, the monitor is
 * set to unowned and up to one thread that may be waiting to lock() the monitor
 * is awakened.
 *
 * This normally should be called by the owning thread of the monitor
 * subsequently to a lock() by the same thread, but this also may be called
 * moncount(mon) times to fully unlock a monitor that was owned by a thread that
 * has been killed.
 *
 * @param mon
 *      The monitor to unlock.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure (@p mon did not specify a valid,
 *      allocated monitor with nonzero lock count).
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

    /* safety check: monitor must be locked at least once  */
    if (monptr->count == 0)
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
