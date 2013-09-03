/**
 * @file kputc.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <kernel.h>
#include "x86uart.h"

syscall kputc(uchar c, device *pdev)
{
    void *pucsr = pdev->csr;

    while ((inb((ulong)pucsr + UART_LSR) & UART_LSR_THRE) == 0)
    {
    }

    outb((ulong)pucsr + UART_DATA, c);

    if (c == '\n')
    {
        while ((inb((ulong)pucsr + UART_LSR) & UART_LSR_THRE) == 0)
        {
        }
        outb((ulong)pucsr + UART_DATA, '\r');
    }

    return OK;
}
