/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "ns16550.h"

void uartHwPutc(void *csr, uchar c)
{
    volatile struct ns16550_uart_csreg *regptr = csr;

    regptr->thr = c;
}
