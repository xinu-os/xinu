/**
 * @file kgetc.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <kernel.h>
#include <uart.h>
#include "x86uart.h"

syscall kgetc(device *pdev)
{
        int irmask;
        volatile struct uart_csreg *pucsr = pdev->csr;
        uchar c;

        irmask = pucsr->ier;        /* Save UART interrupt state.   */
        pucsr->ier = 0;             /* Disable UART interrupts.     */

        while (0 == (pucsr->lsr & UART_LSR_DR))
        {
        }

        /* read character from Receive Holding Register */
        c = pucsr->rbr;

        pucsr->ier = irmask;        /* Restore UART interrupts.     */
        return c;
}
