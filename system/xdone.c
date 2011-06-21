/**
 * @file xdone.c
 * @provides xdone.
 *
 * $Id: xdone.c 2157 2010-01-19 00:40:07Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <gpio.h>

extern void halt(void);

/**
 * Print system completion message as last thread exits.
 */
void xdone(void)
{
    kprintf("\r\n\r\nAll user processes have completed.\r\n\r\n");
#ifdef GPIO_BASE
    gpioLEDOff(GPIO_LED_CISCOWHT);
#endif                          /* GPIO_BASE */
    disable();
    halt();
}
