/**
 * @file platforminit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <mips.h>
#include <platform.h>
#include <uart.h>
#include <stddef.h>
#include <string.h>
#include <pic8259.h>

extern ulong cpuid;             /* Processor id                    */

extern struct platform platform;        /* Platform specific configuration */

/**
 * Determines and stores all platform specific information.
 * @return OK if everything is determined successfully
 */
int platforminit(void)
{
    strlcpy(platform.family, "MIPS 24K", PLT_STRMAX);
    strlcpy(platform.name, "Qemu", PLT_STRMAX);
    platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_DEFAULT);
    platform.minaddr = (void *)KSEG0_BASE;
    platform.clkfreq = CLKFREQ_DEFAULT;
    platform.uart_dll = UART_DLL_DEFAULT;
#ifdef PIC8259
    /* Mask off interrupts until needed */
    uchar *pic = (uchar *)PIC_MASTER_IMR;
    *pic = 0xFF;
    pic = (uchar *)PIC_SLAVE_IMR;
    *pic = 0xFF;
    /* Set all interrupts to be level-triggered, not edge */
    pic = (uchar *)PIC_MASTER_ELCR;
    *pic = 0xFF;
    pic = (uchar *)PIC_SLAVE_ELCR;
    *pic = 0xFF;
#endif
    return OK;
}
