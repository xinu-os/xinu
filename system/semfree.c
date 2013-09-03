/**
 * @file semfree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup semaphores
 *
 * Frees a semaphore.  This can be done even if threads are waiting on it, in
 * which case they will be released and able to run.  However, be careful, since
 * such threads will return from wait()ing on a semaphore that no longer exists
 * and there may be assumptions that no longer hold.
 *
 * @param sem
 *      Semaphore to free (allocated by semcreate()).
 *
 * @return
 *      ::SYSERR if @p sem did not specify a valid semaphore; otherwise ::OK.
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
