/**
 * @file uartPutc.c
 * @provides uartPutc.
 *
 * $Id: uartPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>

/**
 * Write a single character to the UART
 * @param  pdev  pointer to UART device
 * @param  ch    character to write 
 */
devcall uartPutc(device *devptr, char ch)
{
    return uartWrite(devptr, (void *)&ch, 1);
}
