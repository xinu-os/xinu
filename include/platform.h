/**
 * @file platform.h
 *
 * $Id: platform.h 2086 2009-10-06 22:24:27Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stddef.h>

#define PLT_STRMAX 18
/**
 * Various platform specific settings
 */
struct platform
{
    char name[PLT_STRMAX];
    char family[PLT_STRMAX];
    void *maxaddr;
    ulong clkfreq;
    uchar uart_dll;
    uchar uart_irqnum;
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

#endif                          /* _PLATFORM_H_ */
