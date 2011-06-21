/**
 * @file     clkinit.c
 * @provides clkinit.
 *
 * $Id: clkinit.c 2070 2009-09-18 22:36:02Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stddef.h>
#include <platform.h>
#include <interrupt.h>
#include <clock.h>
#include <queue.h>

#if RTCLOCK
ulong clkticks = 0;           /** ticks per second downcounter         */
ulong clktime = 0;            /** current time in seconds              */
qid_typ sleepq;               /** queue of sleeping processes          */

/**
 * Initialize the clock and sleep queue.  This function is called at startup.
 */
void clkinit(void)
{
    sleepq = queinit();         /* initialize sleep queue       */

    clkticks = 0;

#ifdef DETAIL
    kprintf("Time base %dHz, Clock ticks at %dHz\r\n",
            platform.clkfreq, CLKTICKS_PER_SEC);
#endif

    /* register clock interrupt */
    interruptVector[IRQ_TIMER] = clkhandler;
    enable_irq(IRQ_TIMER);
    clkupdate(platform.clkfreq / CLKTICKS_PER_SEC);
}
#endif                          /* RTCLOCK */
