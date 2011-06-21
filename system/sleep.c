/**
 * @file sleep.c
 * @provides sleep.
 *
 * $Id: sleep.c 2094 2009-10-09 22:20:18Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stddef.h>
#include <interrupt.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

/**
 * Delay the calling thread n milliseconds.
 * @param ms number of milliseconds to sleep
 * @return OK if thread is sleeping, SYSERR if no clock
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
