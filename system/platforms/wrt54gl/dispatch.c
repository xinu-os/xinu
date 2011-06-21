/**
 * @file dispatch.c 
 * @provides dispatch.
 *
 * $Id: dispatch.c 2092 2009-10-07 23:08:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <stddef.h>
#include <mips.h>
#include <stdio.h>

const char *interrupts[] = {
    "Software interrupt request 0",
    "Software interrupt request 1",
    "Hardware interrupt request 0, eth1",
    "Hardware interrupt request 1, serial",
    "Hardware interrupt request 2, eth0",
    "Hardware interrupt request 3",
    "Hardware interrupt request 4",
    "Hardware interrupt request 5, timer"
};

/**
 * Dispatcher for interrupt requests.
 *
 * @param cause contents of cause register identifies interrupt
 * @param frame pointer to interrupt frame with saved status
 *
 */
void dispatch(long cause, long *frame)
{
    long irqcode = 0, irqnum = -1;
    irqmask im;
    void (*handler) (void);

    irqcode = (cause & CAUSE_IRQ) >> CAUSE_IRQ_SHIFT;

    /* Calculate which interrupt number this is. */
    while (irqcode)
    {
        irqnum++;
        irqcode = irqcode >> 1;
    }

    /* Check for registered interrupt handler. */
    if (NULL == (handler = interruptVector[irqnum]))
    {
        kprintf
            ("Xinu Interrupt %d uncaught, %s\r\n",
             irqnum, interrupts[irqnum]);
        while (1)
            ;                   /* forever */
    }

    im = disable();             /* Disable interrupts for duration of handler */
    exlreset();                 /* Reset system-wide exception bit */

    (*handler) ();              /* Call device-specific handler */

    exlset();                   /* Set system-wide exception bit */
    restore(im);
}
