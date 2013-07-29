/**
 * @file uartInterrupt.c
 * @provides uartInterrupt
 *
 * $Id: uartInterrupt.c 1369 2008-07-04 00:09:43Z mschul $
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
		if (NULL == puart) { continue; }
		pucsr = puart->csr;
		if (NULL == pucsr) { continue; }

		/* Check interrupt identification register */
#ifdef MIPS
		iir = pucsr->iir;
#elif I386
		iir = inb((ulong)pucsr+UART_IIR);
#endif
		if (iir & UART_IIR_IRQ) { continue; }

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
#ifdef MIPS
			lsr = pucsr->lsr;
#elif I386
			lsr = inb((ulong)pucsr+UART_LSR);
#endif
			puart->lserr++;
			break;

		/* Receiver data available or timed out */
		case UART_IIR_RDA:
		case UART_IIR_RTO:
			puart->iirq++;
			count = 0;
#ifdef MIPS
			while (pucsr->lsr & UART_LSR_DR)
#elif I386
			while (inb((ulong)pucsr+UART_LSR) & UART_LSR_DR)
#endif
			{
#ifdef MIPS
				c = pucsr->buffer;
#elif I386
				c = inb((ulong)pucsr+UART_DATA);
#endif
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
#ifdef MIPS
			lsr = pucsr->lsr;   /* Read from LSR to clear interrupt */
#elif I386
			lsr = inb((ulong)pucsr+UART_LSR);
#endif
			count = 0;
			if (puart->ocount > 0)
			{
				/* Write characters to the lower half of the UART. */
				do
				{
					count++;
					puart->ocount--;
#ifdef MIPS
					pucsr->buffer = puart->out[puart->ostart];
#elif I386
					outb((ulong)pucsr+UART_DATA, puart->out[puart->ostart]);
#endif
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

	return;
}
