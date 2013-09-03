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
    switch (cpuid & PRID_CPU)
    {
    case PRID_CPU_BCM3302:
        strlcpy(platform.family, "Broadcom 3302", PLT_STRMAX);
        switch (cpuid & PRID_REV)
        {
        case PRID_REV_WRT54G:
            strlcpy(platform.name, "Linksys WRT54G", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT54G);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_WRT54G;
            platform.uart_dll = UART_DLL_WRT54G;
            break;
        case PRID_REV_WRT54GL:
            strlcpy(platform.name, "Linksys WRT54GL", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT54GL);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_WRT54GL;
            platform.uart_dll = UART_DLL_WRT54GL;
            break;
        case PRID_REV_WRT350N:
            strlcpy(platform.name, "Linksys WRT350N", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT350N);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_WRT350N;
            platform.uart_dll = UART_DLL_WRT350N;
            break;
        default:
            strlcpy(platform.name, "Unknown Platform", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_DEFAULT);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_DEFAULT;
            platform.uart_dll = UART_DLL_DEFAULT;
            return SYSERR;
        }
        break;
    case PRID_CPU_24K:
        strlcpy(platform.family, "MIPS 24K", PLT_STRMAX);
        switch (cpuid & PRID_REV)
        {
        case PRID_REV_WRT160NL:
            strlcpy(platform.name, "Linksys WRT160NL", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT160NL);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_WRT160NL;
            platform.uart_dll = (CLKFREQ_WRT160NL / 16) / UART_BAUD;
            break;
        default:
            strlcpy(platform.name, "Unknown Platform", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_DEFAULT);
            platform.minaddr = (void *)KSEG0_BASE;
            platform.clkfreq = CLKFREQ_DEFAULT;
            platform.uart_dll = UART_DLL_DEFAULT;
            return SYSERR;
        }
        break;
    }
    return OK;
}
