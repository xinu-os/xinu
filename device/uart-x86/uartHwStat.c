/**
 * @file uartHwStat.c
 */
/* Embedded Xinu, Copyright (C)2009, 2013.  All rights reserved. */

#include <uart.h>
#include <stdio.h>
#include "x86uart.h"

void uartHwStat(void *csr)
{
    struct uart_csreg *pucsr = csr;
    static const char *label[2] = { "OFF", "ON " };
    int uart_ier, level;

    pucsr = csr;

    printf("\n\tINTERRUPT ENABLE:\n");
    printf("\t------------------------------------------\n");

    uart_ier = inb((ulong)pucsr + UART_IER);
    level = (uart_ier & UART_IER_ERBFI) ? 1 : 0;
    printf("\t%s  Receiver FIFO Reached Trigger Level\n", label[level]);

    level = (uart_ier & UART_IER_ETBEI) ? 1 : 0;
    printf("\t%s  Transmitter FIFO Empty\n", label[level]);

    level = (uart_ier & UART_IER_ELSI) ? 1 : 0;
    printf("\t%s  Receiver Error or BREAK\n", label[level]);

    level = (uart_ier & UART_IER_EMSI) ? 1 : 0;
    printf("\t%s  Modem Status", label[level]);

    printf("\n");
}
