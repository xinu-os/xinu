/**
 * @file wait.c
 * @provides wait.
 *
 * $Id: wait.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Make current process wait on a semaphore
 * @param sem  target semaphore
 * @return OK on success, SYSERR on failure
 */
syscall wait(semaphore sem)
{
    register struct sement *semptr;
    register struct thrent *thrptr;
    irqmask im;

    im = disable();
    if (isbadsem(sem))
    {
        restore(im);
        return SYSERR;
    }
    thrptr = &thrtab[thrcurrent];
    semptr = &semtab[sem];
    if (--(semptr->count) < 0)
    {
        thrptr->state = THRWAIT;
        thrptr->sem = sem;
        enqueue(thrcurrent, semptr->queue);
        resched();
    }
    restore(im);
    return OK;
}
