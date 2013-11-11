/**
 * @file monfree.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Free a monitor previously allocated with moncreate().
 *
 * A monitor must only be freed when no thread has it locked -- that is, either
 * the monitor is unowned, or is owned by a thread that has been killed.
 *
 * @param mon
 *      The monitor to free.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure (@p mon did not specify a valid,
 *      allocated monitor).
 */
syscall monfree(monitor mon)
{
    struct monent *monptr;
    irqmask im;

    im = disable();

    /* make sure the specified monitor is valid and allocated  */
    if (isbadmon(mon))
    {
        restore(im);
        return SYSERR;
    }

    monptr = &montab[mon];

    /* free the monitor's semaphore and mark the monitor table entry as free  */
    semfree(monptr->sem);
    monptr->state = MFREE;

    restore(im);
    return OK;
}
