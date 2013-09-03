/**
 * @file watchdog.c
 *
 * This file contains code to set the Watchdog Timer on the BCM2835 (Raspberry
 * Pi).  When enabled, the Watchdog Timer must be refreshed before it expires,
 * otherwise the system resets itself.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <thread.h>

#define PM_BASE                 0x20100000
#define PM_RSTC                (PM_BASE + 0x1c)
#define PM_WDOG                (PM_BASE + 0x24)

#define PM_PASSWORD                    0x5a000000
#define PM_RSTC_WRCFG_CLR              0xffffffcf
#define PM_RSTC_WRCFG_FULL_RESET       0x00000020

#define PM_WDOG_UNITS_PER_SECOND      (1 << 16)
#define PM_WDOG_UNITS_PER_MILLISECOND (PM_WDOG_UNITS_PER_SECOND / 1000)

/**
 * Set the watchdog timer.
 *
 * @param msecs
 *      Milliseconds to wait until the system resets itself.
 *
 * @return
 *      OK
 */
syscall watchdogset(uint msecs)
{
    *(volatile uint*)PM_WDOG = PM_PASSWORD | (msecs * PM_WDOG_UNITS_PER_MILLISECOND);
    uint cur = *(volatile uint*)PM_RSTC;
    *(volatile uint*)PM_RSTC = PM_PASSWORD | (cur & PM_RSTC_WRCFG_CLR) |
                               PM_RSTC_WRCFG_FULL_RESET;
    return OK;
}
