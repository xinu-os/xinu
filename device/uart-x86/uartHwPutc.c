/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "x86uart.h"

void uartHwPutc(void *csr, uchar c)
{
    outb((ulong)csr + UART_DATA, c);
}
