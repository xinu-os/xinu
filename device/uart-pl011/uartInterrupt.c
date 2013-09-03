/**
 * @file uartInterrupt.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <thread.h>
#include "pl011.h"

/**
 * @ingroup uarthardware
 *
 * Handle an interrupt request from a PL011 UART.
 */
interrupt uartInterrupt(void)
{
    uint u;

    /* Set resdefer to prevent other threads from being scheduled before this
     * interrupt handler finishes.  This prevents this interrupt handler from
     * being executed re-entrantly.  */
    extern int resdefer;
    resdefer = 1;

    /* Check for interrupts on each UART.  Note: this assumes all the UARTs in
     * 'uarttab' are PL011 UARTs.  */
    for (u = 0; u < NUART; u++)
    {
        uint mis, count;
        uchar c;
        volatile struct pl011_uart_csreg *regptr;
        struct uart *uartptr;

        /* Get a pointer to the UART structure and a pointer to the UART's
         * hardware registers.  */
        uartptr = &uarttab[u];
        regptr = uartptr->csr;

        /* Check the Masked Interrupt Status register to determine which
         * interrupts have occurred, then handle them.  */
        mis = regptr->mis;
        if (mis & PL011_MIS_TXMIS)
        {
            /* Transmit interrupt is asserted.  If FIFOs are enabled, this
             * happens when the amount of data in the transmit FIFO is less than
             * or equal to the programmed trigger level.  If FIFOs are disabled,
             * this happens if the Tx holding register is empty.  */

            /* Increment number of transmit interrupts received on this UART.
             * */
            uartptr->oirq++;

            /* Explicitly clear the transmit interrupt.  This is necessary
             * because there may not be enough bytes in the output buffer to
             * fill the FIFO greater than the transmit interrupt trigger level.
             * If FIFOs are disabled, this applies if there are 0 bytes to
             * transmit and therefore nothing to fill the Tx holding register
             * with.  */
            regptr->icr = PL011_ICR_TXIC;

            /* If there are bytes pending in the output buffer, write them to
             * the UART until either there are no bytes remaining or there is no
             * space remaining in the transmit FIFO.  (If FIFOs are disabled,
             * the Tx holding register acts like a FIFO of size 1, so the code
             * still works.)  Otherwise, the UART is now idle, so set the
             * "oidle" flag, which will allow the next call to uartWrite() to
             * start transmitting again by writing a byte directly to the
             * hardware.  */
            if (uartptr->ocount > 0)
            {
                count = 0;
                do
                {
                    regptr->dr = uartptr->out[uartptr->ostart];
                    uartptr->ostart = (uartptr->ostart + 1) % UART_OBLEN;
                    uartptr->ocount--;
                    count++;
                } while (!(regptr->fr & PL011_FR_TXFF) && (uartptr->ocount > 0));

                /* One or more bytes were successfully removed from the output
                 * buffer and written to the UART hardware.  Increment the total
                 * number of bytes written to this UART and signal up to @count
                 * threads waiting in uartWrite() to tell them there is now
                 * space in the output buffer.  */
                uartptr->cout += count;
                signaln(uartptr->osema, count);
            }
            else
            {
                uartptr->oidle = TRUE;
            }
        }
        if (mis & PL011_MIS_RXMIS)
        {
            /* Receive interrupt is asserted.  If FIFOs are enabled, this
             * happens when the amount of data in the receive FIFO is greater
             * than or equal to the programmed trigger level.  If FIFOs are
             * disabled, this happens when the Rx holding register was filled
             * with one byte.  */

            /* Increment number of receive interrupts received on this UART.  */
            uartptr->iirq++;

            /* Number of bytes successfully buffered so far.  */
            count = 0;

            /* Read bytes from the receive FIFO until it is empty again.  (If
             * FIFOs are disabled, the Rx holding register acts as a FIFO of
             * size 1, so the code still works.)  */
            do
            {
                /* Get a byte from the UART's receive FIFO.  */
                c = regptr->dr;
                if (uartptr->icount < UART_IBLEN)
                {
                    /* There is space for the byte in the input buffer, so add
                     * it and tally one character received.  */
                    uartptr->in[(uartptr->istart +
                                 uartptr->icount) % UART_IBLEN] = c;
                    uartptr->icount++;
                    count++;
                }
                else
                {
                    /* There is *not* space for the byte in the input buffer, so
                     * ignore it and increment the overrun count.  */
                    uartptr->ovrrn++;
                }
            } while (!(regptr->fr & PL011_FR_RXFE));
            /* The receive interrupt will have been automatically cleared
             * because we read bytes from the receive FIFO until it became
             * empty.  */

            /* Increment cin by the number of bytes successfully buffered and
             * signal up to that many threads that are currently waiting in
             * uartRead() for buffered data to become available.  */
            uartptr->cin += count;
            signaln(uartptr->isema, count);
        }
    }

    /* Now that the UART interrupt handler is finished, we can safely wake up
     * any threads that were signaled.  */
    if (--resdefer > 0)
    {
        resdefer = 0;
        resched();
    }
}
