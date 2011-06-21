/**
 * @file uartPutc.c
 * @provides uartPutc.
 *
 * $Id: uartPutc.c 2103 2009-10-27 01:43:40Z brylow $
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
