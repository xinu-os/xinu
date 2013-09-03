/**
 * @file wakeup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <thread.h>
#include <queue.h>
#include <clock.h>

#if RTCLOCK

/**
 * @ingroup threads
 *
 * Wakeup and ready all threads that have no more time to sleep
 */
void wakeup(void)
{
    while (nonempty(sleepq) && (firstkey(sleepq) <= 0))
    {
        ready(dequeue(sleepq), RESCHED_NO);
    }

    resched();
}

#endif /* RTCLOCK */
