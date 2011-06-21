/**
 * @file uartControl.c
 * @provides uartControl.
 *
 * $Id: uartControl.c 2103 2009-10-27 01:43:40Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <uart.h>
#include <device.h>

/**
 * Control parameters to a UART.
 * @param devptr  pointer to UART device
 * @param func  index of function to run (defined in uart.h)
 * @param arg1  first argument to called function
 * @param arg2  second argument to called function
 */
devcall uartControl(device *devptr, int func, long arg1, long arg2)
{
    struct uart *uartptr;
    char old;

    uartptr = &uarttab[devptr->minor];

    switch (func)
    {

        /* Set input mode flags: arg1 = flags to set */
        /* return = old value of flags               */
    case UART_CTRL_SET_IFLAG:
        old = uartptr->iflags & arg1;
        uartptr->iflags |= arg1;
        return old;

        /* Clear input mode flags: arg1 = flags to clear */
        /* return = old value of flags */
    case UART_CTRL_CLR_IFLAG:
        old = uartptr->iflags & arg1;
        uartptr->iflags &= ~(arg1);
        return old;

        /* Get input flags: return = current value of flags */
    case UART_CTRL_GET_IFLAG:
        return uartptr->iflags;

        /* Set output mode flags: arg1 = flags to set */
        /*  return = old value of flags               */
    case UART_CTRL_SET_OFLAG:
        old = uartptr->oflags & arg1;
        uartptr->oflags |= arg1;
        return old;

        /* Clear output mode flags: arg1 = flags to clear */
        /* return = old value of flags                    */
    case UART_CTRL_CLR_OFLAG:
        old = uartptr->oflags & arg1;
        uartptr->oflags &= ~(arg1);
        return old;

        /* Get output flags: return = current value of flags */
    case UART_CTRL_GET_OFLAG:
        return uartptr->oflags;

        /* Determine if the UART transmitter is idle, return TRUE if idle */
    case UART_CTRL_OUTPUT_IDLE:
        return uartptr->oidle;

    }
    return SYSERR;
}
