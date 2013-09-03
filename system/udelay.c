/**
 * @file udelay.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <conf.h>

#if RTCLOCK

#include <clock.h>
#include <platform.h>

/* The below udelay() implementation is platform-independent and only depends on
 * clkcount() being implemented by the platform-specific code.  However it does
 * assume that the platform clock frequency is an even multiple of 1000000.  */

/**
 * @ingroup timer
 *
 * Busy-waits for a number of microseconds.
 * Only use this function if absolutely necessary.  Normally you should call
 * sleep() so that other threads can use the processor immediately.
 *
 * @param us
 *    Number of microseconds to wait for.
 */
void udelay(ulong us)
{
    /* delay = Number of timer ticks to wait for  */
    ulong delay = (platform.clkfreq / 1000000) * us;

    /* start = Starting tick count  */
    ulong start = clkcount();

    /* end = Ending tick count (may have wrapped around)  */
    ulong target = start + delay;

    /* temporary variable  */
    ulong count;

    if (target >= start)
    {
        /* Normal case:  Wait until tick count is greater than target or has
         * wrapped around to less than start.  */
        while (((count = clkcount()) < target) && (count >= start))
            ;
    }
    else
    {
        /* Wrap around case:  Wait for tick count to wrap around, then wait for
         * tick count to reach target.  */
        while ((count = clkcount()) > start)
            ;
        while ((count = clkcount()) < target)
            ;
    }
}

#endif /* RTCLOCK */
