/**
 * @file uartRead.c
 * @provides uartRead
 *
 * $Id: uartRead.c 1369 2008-07-04 00:09:43Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>
#include <interrupt.h>

/**
 * Read into a buffer from the UART.
 * @param devptr UART device table entry
 * @param buf buffer to read bytes into
 * @param len size of the buffer
 * @return count of bytes read
 */
devcall uartRead(device *devptr, void *buf, uint len)
{
	irqmask im;
	int count = 0;
	char c;
	struct uart *uartptr;
	uchar *buffer = buf;

	/* Setup and error check pointers to structures */
	if (!devptr)
	{
		return SYSERR;
	}
	uartptr = &uarttab[devptr->minor];
	if (!uartptr)
	{
		return SYSERR;
	}

	im = disable();

	/* If in non-blocking mode, ensure there is */
    /* enough input for the entire read request */
	if ((uartptr->iflags & UART_IFLAG_NOBLOCK)
		&& (semcount(uartptr->isema) < len))
	{
		restore(im);
		return SYSERR;
	}

	/* Put each character into the buffer from the input buffer */
	while (count < len)
	{
		/* If in non-blocking mode, ensure there is another byte of input */
		if ((uartptr->iflags & UART_IFLAG_NOBLOCK) 
			&& (semcount(uartptr->isema) < 1))
		{ break; }

		/* Wait for input and read character from the  */
		/* input buffer; Preserve the circular buffer  */
		wait(uartptr->isema);
		c = uartptr->in[uartptr->istart];
		*buffer++ = c;
		uartptr->icount--;
		uartptr->istart = (uartptr->istart + 1) % UART_IBLEN;
		count++;

		/* If echo is enabled, echo the character */
		if (uartptr->iflags & UART_IFLAG_ECHO)
		{ uartPutc(uartptr->dev, c); }
	}

	restore(im);
	return count;
}
