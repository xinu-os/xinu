/**
 * @file uartHwInit.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>
#include <platform.h>
#include "x86uart.h"

devcall uartHwInit(device *devptr)
{
    void *pucsr = devptr->csr;

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

    return OK;
}
