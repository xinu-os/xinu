/**
 * @file uartInterrupt.c
 * @provides uartInterrupt.
 *
 * $Id: uartInterrupt.c 2102 2009-10-26 20:36:13Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <device.h>
#include <uart.h>
#include "ns16550.h"

extern int resdefer;

/**
 * Decode hardware interrupt request from UART device.
 */
interrupt uartInterrupt(void)
{
    int u = 0, iir = 0, count = 0;
    char c;
    struct uart *uartptr = NULL;
    struct uart_csreg *regptr = NULL;

    resdefer = 1;               /* deferral rescheduling. */

    for (u = 0; u < NUART; u++)
    {
        uartptr = &uarttab[u];
        if (NULL == uartptr)
        {
            continue;
        }
        regptr = (struct uart_csreg *)uartptr->csr;
        if (NULL == regptr)
        {
            continue;
        }

        /* Check interrupt identification register */
        iir = regptr->iir;
        if (iir & UART_IIR_IRQ)
        {
            continue;
        }

        /*
         * Decode interrupt cause based upon the value taken from the
         * UART interrupt identification register.  Clear interrupt source,
         * and perform the appropriate handling to coordinate properly
         * with the upper half of the driver.
         */

        /* Decode interrupt cause */
        iir &= UART_IIR_IDMASK;
        switch (iir)
        {
            /* Receiver line status interrupt */
        case UART_IIR_RLSI:
            uartptr->lserr++;
            break;

            /* Receiver data available or timed out */
        case UART_IIR_RDA:
        case UART_IIR_RTO:
            uartptr->iirq++;
            count = 0;
            while (regptr->lsr & UART_LSR_DR)
            {
                c = regptr->buffer;
                if (uartptr->icount < UART_IBLEN)
                {
                    uartptr->in
                        [(uartptr->istart +
                          uartptr->icount) % UART_IBLEN] = c;
                    uartptr->icount++;
                    count++;
                }
                else
                {
                    uartptr->ovrrn++;
                }
            }
            uartptr->cin += count;
            signaln(uartptr->isema, count);
            break;

            /* Transmitter holding register empty */
        case UART_IIR_THRE:
            uartptr->oirq++;
            count = 0;
            if (uartptr->ocount > 0)
            {
                /* Write characters to the lower half of the UART. */
                do
                {
                    count++;
                    uartptr->ocount--;
                    regptr->buffer = uartptr->out[uartptr->ostart];
                    uartptr->ostart = (uartptr->ostart + 1) % UART_OBLEN;
                }
                while ((count < UART_FIFO_LEN) && (uartptr->ocount > 0));
            }

            if (count)
            {
                uartptr->cout += count;
                signaln(uartptr->osema, count);
            }
            /* If no characters were written, set the output idle flag. */
            else
            {
                uartptr->oidle = TRUE;
            }
            break;

            /* Modem status change */
        case UART_IIR_MSC:
            break;
        }
    }

    if (--resdefer > 0)
    {
        resdefer = 0;
        resched();
    }
}
