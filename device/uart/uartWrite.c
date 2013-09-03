/**
 * @file uartWrite.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>

/**
 * @ingroup uartgeneric
 *
 * Write a buffer of data to a UART.
 *
 * Caveat: this operates asynchronously, so the data written may be held in an
 * internal buffer and not yet actually written to the hardware.  The UART
 * driver's lower half (interrupt handler; see uartInterrupt()) is responsible
 * for actually writing the data to the hardware.  Exception: when the UART
 * transmitter is idle, uartWrite() can directly write one byte to the hardware.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 * @param buf
 *      Pointer to the buffer of data to write.
 * @param len
 *      Number of bytes to write.
 *
 * @return
 *      On success, returns the number of bytes written, which normally is @p
 *      len, but may be less than @p len if the UART has been set to
 *      non-blocking mode.  Returns ::SYSERR on other error (currently, only if
 *      uartInit() has not yet been called).
 */
devcall uartWrite(device *devptr, const void *buf, uint len)
{
    irqmask im;
    struct uart *uartptr;
    uint count;

    /* Disable interrupts and get a pointer to the UART structure and a pointer
     * to the UART's hardware registers.  */
    im = disable();
    uartptr = &uarttab[devptr->minor];

    /* Make sure uartInit() has run.  */
    if (NULL == uartptr->csr)
    {
        restore(im);
        return SYSERR;
    }

    /* Attempt to write each byte in the buffer.  */
    for (count = 0; count < len; count++)
    {
        /* Next byte to write.  */
        uchar ch = ((const uchar *)buf)[count];

        /* If the UART transmitter hardware is idle, write the byte directly to
         * the hardware.  Otherwise, put the byte in the output buffer for the
         * lower half (interrupt handler).  This may block if there is no space
         * available in the output buffer.  */
        if (uartptr->oidle)
        {
            uartHwPutc(uartptr->csr, ch);
            uartptr->oidle = FALSE;
            uartptr->cout++;
        }
        else
        {
            /* If the UART is in non-blocking mode, ensure there is space for a
             * byte in the output buffer for the lower half (interrupt handler).
             * If not, return early with a short count.  */
            if ((uartptr->oflags & UART_OFLAG_NOBLOCK) &&
                uartptr->ocount == UART_OBLEN)
            {
                break;
            }
            wait(uartptr->osema);
            uartptr->out[(uartptr->ostart + uartptr->ocount) % UART_OBLEN] = ch;
            uartptr->ocount++;
        }
    }

    /* Restore interrupts and return the number of bytes written.  */
    restore(im);
    return count;
}
