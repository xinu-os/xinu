/**
 * @file dispatch.c 
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <stddef.h>
#include <mips.h>
#include "pic8259.h"
#include <stdio.h>

char *interrupts[] = {
    "Software interrupt request 0",
    "Software interrupt request 1",
    "Hardware interrupt request 0, programmable interrupt controller",
    "Hardware interrupt request 1",
    "Hardware interrupt request 2",
    "Hardware interrupt request 3",
    "Hardware interrupt request 4",
    "Hardware interrupt request 5, timer",
    "Interrupt controller request 0, programmable interval timer",
    "Interrupt controller request 1, keyboard",
    "Interrupt controller request 2",
    "Interrupt controller request 3, serial 1",
    "Interrupt controller request 4, serial 0",
    "Interrupt controller request 5",
    "Interrupt controller request 6",
    "Interrupt controller request 7",
    "Interrupt controller request 8, real-time clock",
    "Interrupt controller request 9, ethernet",
    "Interrupt controller request 10",
    "Interrupt controller request 11",
    "Interrupt controller request 12, mouse",
    "Interrupt controller request 13",
    "Interrupt controller request 14, IDE0",
    "Interrupt controller request 15, IDE1"
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

#ifdef PIC8259
    if (IRQ_HW0 == irqnum)
    {
        uchar *picStat = (uchar *)PIC_MASTER;
        irqcode = *picStat;
        irqnum = 7;
        while (irqcode)
        {
            irqnum++;
            irqcode = irqcode >> 1;
        }
        if (7 == irqnum)
        {
            irqnum = 15;
            picStat = (uchar *)PIC_SLAVE;
            irqcode = *picStat;
            while (irqcode)
            {
                irqnum++;
                irqcode = irqcode >> 1;
            }
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
#ifdef PIC8259
    uint irqmisc;
    uchar *picMask = (uchar *)PIC_MASTER_IMR;
    if (irqnumber >= 16)
    {
        picMask = (uchar *)PIC_SLAVE_IMR;
        irqmisc = irqnumber - 16;
        enable_cpuirq(IRQ_HW0);
        *picMask &= ~(1 << irqmisc);
    }
    else if (irqnumber >= 8)
    {
        irqmisc = irqnumber - 8;
        enable_cpuirq(IRQ_HW0);
        *picMask &= ~(1 << irqmisc);
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
#ifdef PIC8259
    uint irqmisc;
    uchar *picMask = (uchar *)PIC_MASTER_IMR;
    if (irqnumber >= 16)
    {
        picMask = (uchar *)PIC_SLAVE_IMR;
        irqmisc = irqnumber - 16;
        *picMask |= (1 << irqmisc);
    }
    else if (irqnumber >= 8)
    {
        irqmisc = irqnumber - 8;
        *picMask |= (1 << irqmisc);
    }
    else
    {
        disable_cpuirq(irqnumber);
    }
#else
    disable_cpuirq(irqnumber);
#endif
}
