/**
 * @file platforminit.c
 * @provides platforminit.
 *
 * $Id: platforminit.c 2092 2009-10-07 23:08:36Z brylow $
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
        strncpy(platform.family, "Broadcom 3302", PLT_STRMAX);
        switch (cpuid & PRID_REV)
        {
        case PRID_REV_WRT54G:
            strncpy(platform.name, "Linksys WRT54G", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT54G);
            platform.clkfreq = CLKFREQ_WRT54G;
            platform.uart_dll = UART_DLL_WRT54G;
            break;
        case PRID_REV_WRT54GL:
            strncpy(platform.name, "Linksys WRT54GL", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT54GL);
            platform.clkfreq = CLKFREQ_WRT54GL;
            platform.uart_dll = UART_DLL_WRT54GL;
            break;
        case PRID_REV_WRT350N:
            strncpy(platform.name, "Linksys WRT350N", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT350N);
            platform.clkfreq = CLKFREQ_WRT350N;
            platform.uart_dll = UART_DLL_WRT350N;
            break;
        default:
            strncpy(platform.name, "Unknown Platform", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_DEFAULT);
            platform.clkfreq = CLKFREQ_DEFAULT;
            platform.uart_dll = UART_DLL_DEFAULT;
            return SYSERR;
        }
        break;
    case PRID_CPU_24K:
        strncpy(platform.family, "MIPS 24K", PLT_STRMAX);
        switch (cpuid & PRID_REV)
        {
        case PRID_REV_WRT160NL:
            strncpy(platform.name, "Linksys WRT160NL", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_WRT160NL);
            platform.clkfreq = CLKFREQ_WRT160NL;
            platform.uart_dll = (CLKFREQ_WRT160NL / 16) / UART_BAUD;
            break;
        default:
            strncpy(platform.name, "Unknown Platform", PLT_STRMAX);
            platform.maxaddr = (void *)(KSEG0_BASE | MAXADDR_DEFAULT);
            platform.clkfreq = CLKFREQ_DEFAULT;
            platform.uart_dll = UART_DLL_DEFAULT;
            return SYSERR;
        }
        break;
    }
    return OK;
}
