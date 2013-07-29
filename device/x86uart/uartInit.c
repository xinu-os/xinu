/**
 * @file uartInit.c
 * @provides uartInit
 *
 * $Id: uartInit.c 1369 2008-07-04 00:09:43Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <uart.h>
#include "x86uart.h"
#include <stddef.h>
#include <platform.h>
#include <interrupt.h>
#include <device.h>
#include <stdlib.h>

struct uart uarttab[NUART];

/**
 * Initialize UART control and status registers and buffers.
 * @param devptr pointer to a uart device
 */
devcall uartInit(device *devptr)
{
	struct uart *puart;
	struct uart_csreg *pucsr;

	/* Initialize structure pointers */
	puart = &uarttab[devptr->minor];
	puart->dev = devptr;
	puart->csr = (struct uart_csreg *)devptr->csr;
	pucsr = puart->csr;

	/* Initialize statistical counts */
	puart->cout = 0;
	puart->cin  = 0;
	puart->lserr= 0;
	puart->ovrrn= 0;
	puart->iirq = 0;
	puart->oirq = 0;
	
	/* Initialize input buffer */
	puart->isema  = semcreate(0);
	puart->iflags = 0;
	puart->istart = 0;
	puart->icount = 0;
	
	/* Initialize output buffer */
	puart->osema  = semcreate(UART_OBLEN);
	puart->oflags = 0;
	puart->ostart = 0;
	puart->ocount = 0;
	puart->oidle = 1;

#ifdef MIPS
	/* Set baud rate */
	pucsr->lcr = UART_LCR_DLAB;       /* Set Divisor Latch Access Bit */
	pucsr->dll = platform.uart_dll;   /* Set Divisor Latch Low Byte   */
	pucsr->dlm = 0x00;                /* Set Divisor Latch High Byte  */

	pucsr->lcr = UART_LCR_8N1;        /* 8 bit, No Parity, 1 Stop     */
	pucsr->fcr = 0x00;                /* Disable FIFO for now         */
	/* OUT2 is used to control the board's interrupt tri-state        */
	/* buffer. It should be set high to generate interrupts properly. */
	pucsr->mcr = UART_MCR_OUT2;       /* Turn on user-defined OUT2.   */
	
	/* Enable interrupts */
	pucsr->ier = UART_IER_ERBFI | UART_IER_ETBEI | UART_IER_ELSI; 
	
	/* Enable UART FIFOs, clear and set interrupt trigger level       */
	puart->csr->fcr = UART_FCR_EFIFO | UART_FCR_RRESET
						| UART_FCR_TRESET | UART_FCR_TRIG2;
	
	/* Enable processor handling of UART interrupt requests */
	interruptVector[IRQ_UART] = devptr->inintr;
	enable_irq(devptr->inmask);
#elif I386
	/* Set baud rate */
	outb((ulong)pucsr+UART_LCR, UART_LCR_DLAB);  /* divisor latch access */
	outb((ulong)pucsr+UART_DLL, platform.uart_dll); /* divisor latch low */
	                                             /* DLL 12 -> 9600 bps   */
	                                             /* DLL 1 -> 115200 bps  */
	outb((ulong)pucsr+UART_DLM, 0);              /* divisor latch high   */

	outb((ulong)pucsr+UART_LCR, UART_LCR_8N1);   /* 8N1 mode             */
	outb((ulong)pucsr+UART_FCR, 0);              /* Disable FIFO         */
	outb((ulong)pucsr+UART_MCR, UART_MCR_OUT2);  /* User-defined OUT2    */
	/* OUT2 is used to control the board's interrupt tri-state           */
	/* buffer. It should be set high to generate interrupt properly.     */

	/* Enable interrupts */
	outb((ulong)pucsr+UART_IER, UART_IER_ERBFI | UART_IER_ETBEI | UART_IER_ELSI);

	/* Enable UART hardware FIFOs, clear contents and set interrupt trigger level */
	outb((ulong)pucsr+UART_FCR, 
	     UART_FCR_EFIFO | UART_FCR_RRESET | UART_FCR_TRESET | UART_FCR_TRIG2);

    set_handler(IRQBASE+devptr->irq, devptr->intr);
#endif

	return OK;
}
