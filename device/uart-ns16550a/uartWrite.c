/**
 * @file uartWrite.c
 * @provides uartWrite.
 *
 * $Id: uartWrite.c 2103 2009-10-27 01:43:40Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include "ns16550.h"
#include <device.h>
#include <interrupt.h>

/**
 * Write a buffer to the UART.
 *
 * @param devptr  pointer to UART device
 * @param buf   buffer of characters to write
 * @param len   number of characters to write from the buffer
 */
devcall uartWrite(device *devptr, void *buf, uint len)
{
    irqmask im;
    int count = 0;
    struct uart *uartptr;
    volatile struct uart_csreg *regptr;
    uchar *buffer = buf;

    uartptr = &uarttab[devptr->minor];
    regptr = (struct uart_csreg *)uartptr->csr;
    if (NULL == regptr)
    {
        return SYSERR;
    }

    im = disable();

    /* If in non-blocking mode, ensure there is enough space for the entire
     * write request */
    if ((uartptr->oflags & UART_OFLAG_NOBLOCK)
        && (semcount(uartptr->osema) < len))
    {
        restore(im);
        return SYSERR;
    }

    /* Put each character from the buffer into the output buffer for the
     * lower half */
    while (count < len)
    {
        /* If in non-blocking mode, ensure there is another byte of space
         * for output */
        if ((uartptr->oflags & UART_OFLAG_NOBLOCK)
            && (semcount(uartptr->osema) < 1))
        {
            break;
        }

        /* If the UART transmitter hardware is idle, write directly to the
         * hardware */
        if (uartptr->oidle)
        {
            uartptr->oidle = FALSE;
            regptr->thr = *buffer++;
            count++;
            uartptr->cout++;
        }
        /* Wait for space and place character in the output buffer for the
         * lower half; Preserve the circular buffer */
        else
        {
            wait(uartptr->osema);
            uartptr->out[(uartptr->ostart +
                          uartptr->ocount) % UART_OBLEN] = *buffer++;
            count++;
            uartptr->ocount++;
        }
    }

    restore(im);
    return count;
}
