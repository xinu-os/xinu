/**
 * @file monfree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <semaphore.h>
#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Deallocate a monitor.
 * Deallocate its semaphore, releasing any threads
 * in the waiting queue. Reset monitor's count. 
 * Deallocate entry in global monitor table.
 * @param mon  target monitor
 * @return OK on success, SYSERR on failure
 */
syscall monfree(monitor mon)
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
    semfree(monptr->sem);       /* deallocate the monitor's semaphore */
    monptr->owner = NOOWNER;    /* reset owner field */
    monptr->count = 0;          /* reset count for this monitor */
    monptr->state = MFREE;

    restore(im);
    return OK;
}
