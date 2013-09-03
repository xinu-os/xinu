/**
 * @file lock.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <semaphore.h>
#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Current thread attempts to grab the lock on a monitor.
 * If another thread does not hold the lock the current thread owns the lock
 * and increases the monitor's count by one. Otherwise, the thread waits.
 * @param mon  target monitor
 * @return OK on success, SYSERR on failure
 */
syscall lock(monitor mon)
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

    /* if no thread owns the lock, the current thread claims it */
    if (NOOWNER == monptr->owner)
    {
        monptr->owner = thrcurrent;     /* current thread now owns the lock    */
        (monptr->count)++;      /* add 1 "lock" to the monitor's count */
        wait(monptr->sem);      /* this thread owns the semaphore      */
    }
    else
    {
        /* if current thread owns the lock increase count; dont wait on sem */
        if (thrcurrent == monptr->owner)
        {
            (monptr->count)++;
        }
        /* if another thread owns the lock, wait on sem until monitor is free */
        else
        {
            wait(monptr->sem);
            monptr->owner = thrcurrent;
            (monptr->count)++;

        }
    }

    restore(im);
    return OK;
}
