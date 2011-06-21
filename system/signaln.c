/**
 * @file signaln.c
 * @provides signaln.
 *
 * $Id: signaln.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Signal a semaphore n times, releasing n waiting threads.
 * @param sem   target semaphore
 * @param count number of times to signal
 * @return OK on success, SYSERR on failure
 */
syscall signaln(semaphore sem, int count)
{
    register struct sement *semptr;
    irqmask im;

    im = disable();
    if (isbadsem(sem) || (count <= 0))
    {
        restore(im);
        return SYSERR;
    }
    semptr = &semtab[sem];
    for (; count > 0; count--)
    {
        if ((semptr->count++) < 0)
        {
            ready(dequeue(semptr->queue), RESCHED_NO);
        }
    }
    resched();
    restore(im);
    return OK;
}
