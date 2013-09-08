/**
 * @file dispatch.c 
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <stddef.h>
#include <mips.h>
#include "ar9130.h"
#include <stdio.h>

char *interrupts[] = {
    "Software interrupt request 0",
    "Software interrupt request 1",
    "Hardware interrupt request 0, wmac",
    "Hardware interrupt request 1, usb",
    "Hardware interrupt request 2, eth0",
    "Hardware interrupt request 3, eth1",
    "Hardware interrupt request 4, misc",
    "Hardware interrupt request 5, timer",
    "Miscellaneous interrupt request 0, timer",
    "Miscellaneous interrupt request 1, error",
    "Miscellaneous interrupt request 2, gpio",
    "Miscellaneous interrupt request 3, uart",
    "Miscellaneous interrupt request 4, watchdog",
    "Miscellaneous interrupt request 5, perf",
    "Miscellaneous interrupt request 6, reserved",
    "Miscellaneous interrupt request 7, mbox",
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

#ifdef IRQ_ATH_MISC
    if (IRQ_ATH_MISC == irqnum)
    {
        ulong *miscStat = (ulong *)RST_MISC_INTERRUPT_STATUS;
        ulong *miscMask = (ulong *)RST_MISC_INTERRUPT_MASK;
        irqcode = *miscStat & (*miscMask & RST_MISC_IRQ_MASK);
        if (!irqcode)
            return;             /* Spurious AHB interrupt. */
        irqnum = 7;
        while (irqcode)
        {
            irqnum++;
            irqcode = irqcode >> 1;
        }
    }
#endif

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

/**
 * Enable IRQ number.
 *
 * @param irqnumber Request number to enable.
 *
 */
void enable_irq(irqmask irqnumber)
{
#ifdef IRQ_ATH_MISC
    int irqmisc;
    ulong *miscMask = (ulong *)RST_MISC_INTERRUPT_MASK;
    if (irqnumber >= 8)
    {
        irqmisc = irqnumber - 8;
        enable_cpuirq(IRQ_ATH_MISC);
        *miscMask |= (1 << irqmisc);
    }
    else
    {
        enable_cpuirq(irqnumber);
    }
#else
    enable_cpuirq(irqnumber);
#endif
}

/**
 * Disable IRQ number.
 *
 * @param irqnumber Request number to enable.
 *
 */
void disable_irq(irqmask irqnumber)
{
#ifdef IRQ_ATH_MISC
    int irqmisc;
    ulong *miscMask = (ulong *)RST_MISC_INTERRUPT_MASK;
    if (irqnumber >= 8)
    {
        irqmisc = irqnumber - 8;
        *miscMask &= ~(1UL << irqmisc);
    }
    else
    {
        disable_cpuirq(irqnumber);
    }
#else
    disable_cpuirq(irqnumber);
#endif
}
