/**
 * @file uartGetc.c
 * @provides uartGetc.
 *
 * $Id: uartGetc.c 2103 2009-10-27 01:43:40Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>

/**
 * Read a single character from UART.
 * @param pdev pointer to UART device
 */
devcall uartGetc(device *devptr)
{
    uchar ch = 0;

    uartRead(devptr, &ch, 1);
    return ch;
}
