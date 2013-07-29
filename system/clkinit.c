/**
 * @file     clkinit.c
 * @provides clkinit.
 *
 * $Id: clkinit.c 2533 2011-07-26 20:06:23Z pspillan $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stddef.h>
#include <platform.h>
#include <interrupt.h>
#include <clock.h>
#include <queue.h>

#if RTCLOCK
#define CLOCKBASE 0x40         /* I/O base port of clock chip for x86 */
#define CLOCKCTL (CLOCKBASE+3) /* chip CSW I/O port for x86           */
ulong time_intr_freq = 0;     /** frequency of XINU clock interrupt   */
ulong clkticks = 0;           /** ticks per second downcounter         */
ulong clktime = 0;            /** current time in seconds              */
qid_typ sleepq;               /** queue of sleeping processes          */

extern void clockIRQ(void);

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
    /*
#ifndef I386
    interruptVector[IRQ_TIMER] = clkhandler;
    enable_irq(IRQ_TIMER);
    clkupdate(platform.clkfreq / CLKTICKS_PER_SEC);
#else
    */
	time_intr_freq = platform.clkfreq / CLKTICKS_PER_SEC;
	outb(CLOCKCTL, 0x34);
	/* LSB then MSB */
	outb(CLOCKBASE, time_intr_freq);
	outb(CLOCKBASE, time_intr_freq >> 8);
	outb(CLOCKBASE, time_intr_freq >> 8); /* why??? */

	set_evec(IRQBASE, (ulong)clockIRQ);
//#endif

}
#endif /* RTCLOCK */
