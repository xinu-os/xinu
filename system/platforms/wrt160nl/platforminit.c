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

extern ulong cpuid;             /* Processor id                    */

extern struct platform platform;        /* Platform specific configuration */

/**
 * Determines and stores all platform specific information.
 * @return OK if everything is determined successfully
 */
int platforminit(void)
{
    strncpy(platform.family, "MIPS 24K", PLT_STRMAX);
    strncpy(platform.name, "Linksys WRT160NL", PLT_STRMAX);
    platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT160NL);
    platform.clkfreq = CLKFREQ_WRT160NL;
    platform.uart_dll = (CLKFREQ_WRT160NL / 16) / UART_BAUD;
    return OK;
}
