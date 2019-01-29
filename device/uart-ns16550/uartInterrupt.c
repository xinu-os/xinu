/**
 * @file uartInterrupt.c
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <device.h>
#include <uart.h>
#include "ns16550.h"

extern int resdefer;

/**
 * @ingroup uarthardware
 *
 * Decode hardware interrupt request from UART device.
 */
interrupt uartInterruptNoRsch(void)
{
    int u = 0, iir = 0, lsr = 0, count = 0;
    char c;
    struct uart *uartptr = NULL;
    struct ns16550_uart_csreg *regptr = NULL;

    resdefer = 1;               /* deferral rescheduling. */

    for (u = 0; u < NUART; u++)
    {
        uartptr = &uarttab[u];
        if (NULL == uartptr)
        {
            continue;
        }
        regptr = (struct ns16550_uart_csreg *)uartptr->csr;
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
            lsr = regptr->lsr;
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

            /* Fall through -- Rx status trumps Tx status on Qemu. */

            /* Transmitter holding register empty */
        case UART_IIR_THRE:
            lsr = regptr->lsr;  /* Read from LSR to clear interrupt */
            if (!(lsr & UART_LSR_THRE))
                break;
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
}

/**
 * @ingroup uarthardware
 *
 * Decode hardware interrupt request from UART device and call
 * resched() if needed. Normally used as UART interrupt handler except
 * on the architectures where devices share interrupts.
 *
 */
interrupt uartInterrupt(void)
{
    uartInterruptNoRsch();
    if (--resdefer > 0) 
    { 
	 resdefer = 0; 
	 resched();
    }
}
