/**
 * @file xdone.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <gpio.h>

extern void halt(void);

/**
 * @ingroup misc
 *
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
