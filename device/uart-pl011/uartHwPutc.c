/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "pl011.h"

void uartHwPutc(void *csr, uchar c)
{
    volatile struct pl011_uart_csreg *regptr = csr;

    regptr->dr = c;
}
