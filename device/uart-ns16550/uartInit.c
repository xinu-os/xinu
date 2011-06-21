/**
 * @file uartInit.c
 * @provides uartInit.
 *
 * $Id: uartInit.c 2102 2009-10-26 20:36:13Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <uart.h>
#include "ns16550.h"
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
    struct uart *uartptr;
    struct uart_csreg *regptr;

    /* Initialize structure pointers */
    uartptr = &uarttab[devptr->minor];
    uartptr->dev = devptr;
    uartptr->csr = devptr->csr;
    regptr = (struct uart_csreg *)uartptr->csr;

    /* Initialize statistical counts */
    uartptr->cout = 0;
    uartptr->cin = 0;
    uartptr->lserr = 0;
    uartptr->ovrrn = 0;
    uartptr->iirq = 0;
    uartptr->oirq = 0;

    /* Initialize input buffer */
    uartptr->isema = semcreate(0);
    uartptr->iflags = 0;
    uartptr->istart = 0;
    uartptr->icount = 0;

    /* Initialize output buffer */
    uartptr->osema = semcreate(UART_OBLEN);
    uartptr->oflags = 0;
    uartptr->ostart = 0;
    uartptr->ocount = 0;
    uartptr->oidle = 1;

    /* Set baud rate */
    regptr->lcr = UART_LCR_DLAB;        /* Set Divisor Latch Access Bit */
    regptr->dll = platform.uart_dll;    /* Set Divisor Latch Low Byte   */
    regptr->dlm = 0x00;         /* Set Divisor Latch High Byte  */

    regptr->lcr = UART_LCR_8N1; /* 8 bit, No Parity, 1 Stop     */
    regptr->fcr = 0x00;         /* Disable FIFO for now         */
    /* OUT2 is used to control the board's interrupt tri-state        */
    /* buffer. It should be set high to generate interrupts properly. */
    regptr->mcr = UART_MCR_OUT2;        /* Turn on user-defined OUT2.   */

    /* Enable interrupts */
    regptr->ier = UART_IER_ERBFI | UART_IER_ETBEI | UART_IER_ELSI;

    /* Enable UART FIFOs, clear and set interrupt trigger level       */
    regptr->fcr = UART_FCR_EFIFO | UART_FCR_RRESET
        | UART_FCR_TRESET | UART_FCR_TRIG2;

    /* Enable processor handling of UART interrupt requests */
    interruptVector[devptr->irq] = devptr->intr;
    enable_irq(devptr->irq);

    return OK;
}
