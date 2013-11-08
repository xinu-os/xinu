/**
 * @file platform.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stddef.h>
#include <stdarg.h>

/** Maximum length of platform name and family strings, including the
 * null-terminator.  */
#define PLT_STRMAX 18

/**
 * Various platform-specific parameters filled in at boot time by
 * platforminit().  When platforminit() is called this structure will initially
 * be all 0's.
 */
struct platform
{
    /** Name of the platform as a null-terminated string.  It is
     * platform-specific what is considered a "name" and what is considered a
     * "family".  */
    char name[PLT_STRMAX];

    /** Family of the platform as a null-terminated string.  It is
     * platform-specific what is considered a "name" and what is considered a
     * "family".  */
    char family[PLT_STRMAX];

    /** Minimum physical address available to the CPU.  platforminit() can leave
     * this value alone if available physical memory starts at 0.  */
    void *minaddr;

    /** One byte past the last byte physically addressable by the CPU.  Must be
     * set by platforminit().  */
    void *maxaddr;

    /** Frequency of the system timer in cycles per second.  Must be set by
     * platforminit() if RTCLOCK is enabled.  This is the frequency at which the
     * values returned by clkcount() change.  */
    ulong clkfreq;

    /** UART Divisor Latch LS.  platforminit() only needs to set this if
     * required by the UART driver.  */
    uchar uart_dll;

    /** Number of the UART IRQ line.  platforminit() only needs to set this if
     * required by the UART driver.  */
    uchar uart_irqnum;

    /** Low word of the board's serial number.  Setting this is optional; it's
     * currently only used by the SMSC LAN9512 driver.  */
    uint serial_low;

    /** High word of the board's serial number.  Setting this is optional; it's
     * currently only used by the SMSC LAN9512 driver.  */
    uint serial_high;
};

extern struct platform platform;

/* Max RAM addresses */
#define MAXADDR_DEFAULT  0x00800000 /**< default  8MB RAM */
#define MAXADDR_WRT54G   0x00800000 /**< G    has 8MB RAM */
#define MAXADDR_WRT54GL  0x01000000 /**< GL   has 16MB RAM */
#define MAXADDR_WRT350N  0x02000000 /**< 350N has 32MB RAM */
#define MAXADDR_WRT160NL 0x02000000 /**< 160NL has 32MB RAM */

/* Time Base Frequency */
#define CLKFREQ_DEFAULT  100000000
#define CLKFREQ_WRT54G   120000000
#define CLKFREQ_WRT54GL  100000000
#define CLKFREQ_WRT350N  150000000
#define CLKFREQ_WRT160NL 200000000

/* UART DLL (Divisor Latch LS) */
#define UART_DLL_DEFAULT    0x0B
#define UART_DLL_WRT54G     0x0E
#define UART_DLL_WRT54GL    0x0B
#define UART_DLL_WRT350N    0x29

/* Used internally by create()  */
void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, uint nargs, va_list ap);

int platforminit(void);

#endif                          /* _PLATFORM_H_ */
