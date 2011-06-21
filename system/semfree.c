/**
 * @file semfree.c
 * @provides semfree.
 *
 * $Id: semfree.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Deallocate a semaphore.
 * Reset the semaphore count, releasing any threads
 * in the waiting queue.  Deallocate entry in global
 * semaphore table.
 * @param sem  target semaphore
 * @return OK on success, SYSERR on failure
 */
syscall semfree(semaphore sem)
{
    register struct sement *semptr;
    irqmask im;
    tid_typ tid;

    im = disable();
    if (isbadsem(sem))
    {
        restore(im);
        return SYSERR;
    }

    semptr = &semtab[sem];
    while (nonempty(semptr->queue))
    {
        tid = dequeue(semptr->queue);   /* free waiting threads */
        ready(tid, RESCHED_NO);
    }

    semptr->count = 0;
    semptr->state = SFREE;
    restore(im);
    return OK;
}
