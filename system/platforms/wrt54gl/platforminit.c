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
    strlcpy(platform.family, "Broadcom 3302", PLT_STRMAX);
    strlcpy(platform.name, "Linksys WRT54GL", PLT_STRMAX);
    platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT54GL);
    platform.minaddr = (void *)KSEG0_BASE;
    platform.clkfreq = CLKFREQ_WRT54GL;
    platform.uart_dll = UART_DLL_WRT54GL;
    return OK;
}
