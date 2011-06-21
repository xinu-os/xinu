/**
 * @file signal.c
 * @provides signal.
 *
 * $Id: signal.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * signal a semaphore, releasing one waiting thread
 * @param sem  target semaphore
 * @return OK on success, SYSERR on failure
 */
syscall signal(semaphore sem)
{
    register struct sement *semptr;
    irqmask im;

    im = disable();
    if (isbadsem(sem))
    {
        restore(im);
        return SYSERR;
    }
    semptr = &semtab[sem];
    if ((semptr->count++) < 0)
    {
        ready(dequeue(semptr->queue), RESCHED_YES);
    }
    restore(im);
    return OK;
}
