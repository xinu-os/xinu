/**
 * @file uartHwInit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>
#include <platform.h>
#include "ns16550.h"

devcall uartHwInit(device *devptr)
{
    volatile struct ns16550_uart_csreg *regptr = devptr->csr;

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
