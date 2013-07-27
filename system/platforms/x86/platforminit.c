/**
 * @file platforminit.c
 * @provides platforminit.
 *
 * $Id: platforminit.c 2167 2010-02-11 00:34:32Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <mips.h>
#include <platform.h>
#include <uart.h>
#include <stddef.h>
#include <string.h>
#include <asm-i386/icu.h>
#include <interrupt.h>

extern ulong cpuid;             /* Processor id                    */

extern struct platform platform;        /* Platform specific configuration */

/* prototypes */
void lidt(void);

/* interrupt variables */
extern short girmask;
extern long defevec[];
extern struct segtr idtr;
extern struct idt idt[NID];

/**
 * Determines and stores all platform specific information.
 * @return OK if everything is determined successfully
 */
extern int platforminit(void)
{
    int i;

    /* Setup platform data                                 */
	strncpy(platform.name, "Intel x86", PLT_STRMAX);
    platform.maxaddr = (void *)0x1000000;
    platform.clkfreq = 1190000;
    platform.uart_dll = 1;

    /* Setup Intel 8259a programmable interrupt controller */
    /* Master device                                       */
    outb(ICU1,   0x11); /* ICW1: icw4 needed               */
    outb(ICU1+1, 0x20); /* ICW2: base ivec 32              */
    outb(ICU1+1, 0x4);  /* ICW3: cascade on irq2           */
    outb(ICU1+1, 0x1);  /* ICW4: buf. master, 808x mode    */
    outb(ICU1,   0xb);  /* OCW3: set ISR on read           */

    /* Slave device                                        */
    outb(ICU2,   0x11); /* ICW1: icw4 needed               */
    outb(ICU2+1, 0x28); /* ICW2: base ivec 40              */
    outb(ICU2+1, 0x2);  /* ICW3: slave on irq2             */
    outb(ICU2+1, 0xb);  /* ICW4: buf. slave, 808x mode     */
    outb(ICU2,   0xb);  /* OCW3: set ISR on read           */

    disable();

    /* Set initial interrupt entry vectors                 */
    /* Initialize interrupts variables                     */
    for (i = 0; i < NID; i++)
    { set_evec(i, (long)defevec[i]); }

    /* girmask masks bus interrupts with default handler   */
    girmask = 0xfffb;
    lidt();

    return OK; 
}
