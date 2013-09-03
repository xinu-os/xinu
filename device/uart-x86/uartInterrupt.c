/**
 * @file uartInterrupt.c
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <uart.h>
#include "x86uart.h"
#include <semaphore.h>
#include <interrupt.h>
#include <asm-i386/icu.h>

/**
 * Decode hardware interrupt request from UART device.
 */
interrupt uartInterrupt(void)
{
    int u = 0, iir = 0, lsr = 0, count = 0;
    char c;
    struct uart       *puart = NULL;
    struct uart_csreg *pucsr = NULL;

    for (u = 0; u < NUART; u++)
    {
        puart = &uarttab[u];
        pucsr = puart->csr;
        if (NULL == pucsr)
        {
            continue;
        }

        /* Check interrupt identification register */
        iir = inb((ulong)pucsr+UART_IIR);
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
            lsr = inb((ulong)pucsr+UART_LSR);
            puart->lserr++;
            break;

        /* Receiver data available or timed out */
        case UART_IIR_RDA:
        case UART_IIR_RTO:
            puart->iirq++;
            count = 0;
            while (inb((ulong)pucsr+UART_LSR) & UART_LSR_DR)
            {
                c = inb((ulong)pucsr+UART_DATA);
                if (puart->icount < UART_IBLEN)
                {
                    puart->in[(puart->istart+puart->icount) % UART_IBLEN] = c;
                    puart->icount++;
                    count++;
                }
                else
                {
                    puart->ovrrn++;
                }
            }
            puart->cin += count;
            signaln(puart->isema, count);
            break;

        /* Transmitter holding register empty */
        case UART_IIR_THRE:
            puart->oirq++;
            lsr = inb((ulong)pucsr+UART_LSR);  /* Read from LSR to clear interrupt */
            count = 0;
            if (puart->ocount > 0)
            {
                /* Write characters to the lower half of the UART. */
                do
                {
                    count++;
                    puart->ocount--;
                    outb((ulong)pucsr+UART_DATA, puart->out[puart->ostart]);
                    puart->ostart = (puart->ostart + 1) % UART_OBLEN;
                } while ((count < UART_FIFO_LEN) && (puart->ocount > 0));
            }

            if (count)
            {
                puart->cout += count;
                signaln(puart->osema, count);
            }
            /* If no characters were written, set the output idle flag. */
            else
            {
                puart->oidle = TRUE;
            }
            break;

        /* Modem status change */
        case UART_IIR_MSC:
            break;
        }
    }
}
