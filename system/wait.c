/**
 * @file wait.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup semaphores
 *
 * Wait on a semaphore.
 *
 * If the semaphore's count is positive, it will be decremented and this
 * function will return immediately.  Otherwise, the currently running thread
 * will be put to sleep until the semaphore is signaled with signal() or
 * signaln(), or freed with semfree().
 *
 * @param sem
 *      Semaphore to wait on.
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail if @p
 *      sem did not specify a valid semaphore.
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
