/**
 * @file uartWrite.c
 * @provides uartWrite
 *
 * $Id: uartWrite.c 1369 2008-07-04 00:09:43Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include "x86uart.h"
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
	irqmask ps;
	int count = 0;
	struct uart *uartptr;
	volatile struct uart_csreg *pucsr;
	uchar *buffer = buf;

	/* Setup and error check pointers to structures */
	if (NULL == devptr)  { return SYSERR; }

	uartptr = &uarttab[devptr->minor];
	if (NULL == uartptr) { return SYSERR; } 

	pucsr = uartptr->csr;
	if (NULL == pucsr) { return SYSERR; }

	ps = disable();

	/* If in non-blocking mode, ensure there is enough space for the entire
	 * write request */
	if ((uartptr->oflags & UART_OFLAG_NOBLOCK)
		&& (semcount(uartptr->osema) < len))
	{
		restore(ps);
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
		{ break; }

		/* If the UART transmitter hardware is idle, write directly to the
		 * hardware */
		if (uartptr->oidle)
		{
			uartptr->oidle = FALSE;
#ifdef MIPS
			pucsr->thr = *buffer++;
#elif I386
			outb((ulong)pucsr+UART_DATA, *buffer++);
#endif
			count++;
			uartptr->cout++;
		}
		/* Wait for space and place character in the output buffer for the
		 * lower half; Preserve the circular buffer */
		else
		{
			wait(uartptr->osema);
			uartptr->out[(uartptr->ostart + uartptr->ocount) % UART_OBLEN] = *buffer++;
			count++;
			uartptr->ocount++;
		}
	}

	restore(ps);
	return count;
}
