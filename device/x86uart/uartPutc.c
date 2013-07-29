/**
 * @file uartPutc.c
 * @provides uartPutc
 *
 * $Id: uartPutc.c 413 2008-01-11 21:18:29Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>

/**
 * Write a single character to the UART
 * @param  devptr  pointer to UART device
 * @param  ch    character to write 
 */
devcall uartPutc(device *devptr, char ch)
{
	return uartWrite(devptr, &ch, 1);
}

