/**
 * @file kprintf.c
 * @provides kputc, kprintf
 *
 * $Id: kprintf.c 1693 2009-02-06 02:30:18Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <uart.h>
#include "x86uart.h"

#define DATA 0
#define LSR  5

/**
 * kputc - perform a synchronous kernel write to the console tty
 * @param *pdev pointer to device on which to write character
 * @param c character to write
 * @return c on success, SYSERR on failure
 */
syscall kputc(device *dptr, unsigned char c)
{
	int csr = UART_BASE;

	while ((inb(csr+LSR) & UART_LSR_THRE) == 0)
	/* empty */;

	outb(csr+DATA, c);

	if (c == '\n') {
	while ((inb(csr+LSR) & UART_LSR_THRE) == 0)
		/* empty */;
	outb(csr+DATA, '\r');
	}

	return OK;
}

/**
 * kgetc - perform a synchronous kernel read from the console tty
 * @param *pdev pointer to device on which to write character
 * @param c character to write
 * @return c on success, SYSERR on failure
 */
syscall kgetc(device *pdev)
{
        int irmask;
        struct uart *puart;
        volatile struct uart_csreg *pucsr;
        uchar c;

        if (NULL == pdev) { return SYSERR; }
        puart  = &uarttab[pdev->minor];
        if (NULL == puart) { puart = &uarttab[0]; }
        pucsr = puart->csr;
        if (NULL == pucsr) { pucsr = (struct uart_csreg *)pdev->csr; }

        irmask = pucsr->ier;        /* Save UART interrupt state.   */
        pucsr->ier = 0;             /* Disable UART interrupts.     */

        while (0 == (pucsr->lsr & UART_LSR_DR))
        ;
        /* read character from Receive Holding Register */
        c = pucsr->rbr;

        pucsr->ier = irmask;        /* Restore UART interrupts.     */
        return c;
}

/**
 * kprintf - kernel printf: formatted, unbuffered output to CONSOLE
 * @param *fmt pointer to string being printed
 * @return OK on success
 */
syscall kprintf(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_doprnt(fmt, ap, (int (*)(int, int))kputc, NULL);
	va_end(ap);
	return OK;
}
