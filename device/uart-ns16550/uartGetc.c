/**
 * @file uartGetc.c
 * @provides uartGetc.
 *
 * $Id: uartGetc.c 2077 2009-09-24 23:58:54Z mschul $
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
