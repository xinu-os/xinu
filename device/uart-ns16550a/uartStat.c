/**
 * @file     uartStat.c
 * @provides uartStat.
 *
 * $Id: uartStat.c 2103 2009-10-27 01:43:40Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include "ns16550.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Provides information about the current status of a UART.
 * @param uartnum number of device in uarttab
 */

void uartStat(ushort uartnum)
{
    struct uart *puart;         /* pointer to uart entry     */
    struct uart_csreg *pucsr;   /* pointer to uart registers */
    char *label[2] = { "OFF", "ON " };

    /* Initialize pointers */
    puart = &uarttab[uartnum];
    pucsr = (struct uart_csreg *)puart->csr;

    if (NULL == puart)
    {
        return;
    }
    if (NULL == pucsr)
    {
        return;
    }

    printf("%s:\n", (puart->dev)->name);

    /* UART statistics */
    printf("\tSTATISTICS:\n");
    printf("\t------------------------------------------\n");
    printf("\t%8d Characters Output\n", puart->cout);
    printf("\t%8d Characters Input\n", puart->cin);
    printf("\t%8d Characters Overrun\n", puart->ovrrn);
    printf("\t%8d Receiver Error Count\n", puart->lserr);
    printf("\t%8d Output IRQ Count\n", puart->oirq);
    printf("\t%8d Input IRQ Count\n", puart->iirq);

    /* Interrupt enable status */
    int enabled;

    printf("\n\tINTERRUPT ENABLE:\n");
    printf("\t------------------------------------------\n");

    enabled = (pucsr->ier & UART_IER_ERBFI) ? 1 : 0;
    printf("\t%s  Receiver FIFO Reached Trigger Level\n", label[enabled]);

    enabled = (pucsr->ier & UART_IER_ETBEI) ? 1 : 0;
    printf("\t%s  Transmitter FIFO Empty\n", label[enabled]);

    enabled = (pucsr->ier & UART_IER_ELSI) ? 1 : 0;
    printf("\t%s  Receiver Error or BREAK\n", label[enabled]);

    enabled = (pucsr->ier & UART_IER_EMSI) ? 1 : 0;
    printf("\t%s  Modem Status", label[enabled]);

    printf("\n");
}
