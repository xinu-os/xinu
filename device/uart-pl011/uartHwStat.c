/**
 * @file uartHwStat.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <stdio.h>
#include "pl011.h"

void uartHwStat(void *csr)
{
    static const char * const label[2] = { "OFF", "ON " };
    uint enabled;
    volatile struct pl011_uart_csreg *regptr = csr;

    printf("\n\tINTERRUPT ENABLE:\n");
    printf("\t------------------------------------------\n");

    enabled = (regptr->imsc & PL011_IMSC_RXIM) ? 1 : 0;
    printf("\t%s  Receiver FIFO Reached Trigger Level\n", label[enabled]);

    enabled = (regptr->imsc & PL011_IMSC_TXIM) ? 1 : 0;
    printf("\t%s  Transmitter FIFO Empty\n", label[enabled]);
}
