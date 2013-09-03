/**
 * @file sleep.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stddef.h>
#include <interrupt.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

/**
 * @ingroup threads
 *
 * Yields the processor for the specified number of milliseconds, allowing other
 * threads to be scheduled.
 *
 * @param ms number of milliseconds to sleep
 *
 * @return
 *      If successful, the thread will sleep for the specified number of
 *      milliseconds, then return ::OK.  Otherwise, ::SYSERR will be returned.
 *      If a system timer is not supported, ::SYSERR will always returned.
 */
syscall sleep(uint ms)
{
#if RTCLOCK
    irqmask im;
    int ticks = 0;

    ticks = (ms * CLKTICKS_PER_SEC) / 1000;

    im = disable();
    if (ticks > 0)
    {
        if (SYSERR == insertd(thrcurrent, sleepq, ticks))
        {
            restore(im);
            return SYSERR;
        }
        thrtab[thrcurrent].state = THRSLEEP;
    }

    resched();
    restore(im);
    return OK;
#else
    return SYSERR;
#endif
}
