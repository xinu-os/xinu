/**
 * @file uartHwStat.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <stdio.h>
#include "ns16550.h"

void uartHwStat(void *csr)
{
    static const char * const label[2] = { "OFF", "ON " };
    uint enabled;
    volatile struct ns16550_uart_csreg *regptr = csr;

    printf("\n\tINTERRUPT ENABLE:\n");
    printf("\t------------------------------------------\n");

    enabled = (regptr->ier & UART_IER_ERBFI) ? 1 : 0;
    printf("\t%s  Receiver FIFO Reached Trigger Level\n", label[enabled]);

    enabled = (regptr->ier & UART_IER_ETBEI) ? 1 : 0;
    printf("\t%s  Transmitter FIFO Empty\n", label[enabled]);

    enabled = (regptr->ier & UART_IER_ELSI) ? 1 : 0;
    printf("\t%s  Receiver Error or BREAK\n", label[enabled]);

    enabled = (regptr->ier & UART_IER_EMSI) ? 1 : 0;
    printf("\t%s  Modem Status", label[enabled]);
}
