/**
 * @file uartRead.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>

/**
 * @ingroup uartgeneric
 *
 * Reads data from a UART.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 * @param buf
 *      Pointer to a buffer into which to place the read data.
 * @param len
 *      Maximum number of bytes of data to read.
 *
 * @return
 *      On success, returns the number of bytes read, which normally is @p len,
 *      but may be less than @p len if the UART has been set to non-blocking
 *      mode.  Returns SYSERR on other error (currently, only if uartInit() has
 *      not yet been called).
 */
devcall uartRead(device *devptr, void *buf, uint len)
{
    irqmask im;
    struct uart *uartptr;
    uint count;
    uchar c;

    /* Disable interrupts and get a pointer to the UART structure.  */
    im = disable();
    uartptr = &uarttab[devptr->minor];

    /* Make sure uartInit() has run.  */
    if (NULL == uartptr->csr)
    {
        restore(im);
        return SYSERR;
    }

    /* Attempt to read each byte requested.  */
    for (count = 0; count < len; count++)
    {
        /* If the UART is in non-blocking mode, ensure there is a byte available
         * in the input buffer from the lower half (interrupt handler).  If not,
         * return early with a short count.  */
        if ((uartptr->iflags & UART_IFLAG_NOBLOCK) && uartptr->icount == 0)
        {
            break;
        }

        /* Wait for there to be at least one byte in the input buffer from the
         * lower half (interrupt handler), then remove it.  */
        wait(uartptr->isema);
        c = uartptr->in[uartptr->istart];
        ((uchar*)buf)[count] = c;
        uartptr->icount--;
        uartptr->istart = (uartptr->istart + 1) % UART_IBLEN;

        /* If the UART is in echo mode, echo the byte back to the UART.  */
        if (uartptr->iflags & UART_IFLAG_ECHO)
        {
            uartPutc(uartptr->dev, c);
        }
    }

    /* Restore interrupts and return the number of bytes read.  */
    restore(im);
    return count;
}
