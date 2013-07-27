/**
 * @file x86uart.h
 *
 * $Id: uart.h 1371 2008-07-04 18:09:37Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#ifndef _X86UART_H_
#define _x86UART_H_

#include <device.h>
#include <semaphore.h>
#include <stddef.h>

/**
 * Control and status registers for the 16550 UART.  This structure is
 * mapped directly to the base address for the CSR.
 */
struct uart_csreg
{
    volatile uchar buffer;    /**< receive buffer (read only)            */
                              /**<  OR transmit hold (write only)        */
    volatile uchar ier;       /**< interrupt enable                      */
    volatile uchar iir;       /**< interrupt ident (read only)           */
                              /**<  OR FIFO control (write only)         */
    volatile uchar lcr;       /**< line control                          */
    volatile uchar mcr;       /**< modem control                         */
    volatile uchar lsr;       /**< line status                           */
    volatile uchar msr;       /**< modem status                          */
    volatile uchar scr;       /**< scratch                               */
};

/* Base address for TTY */
#define UART_BASE       0x3f8

/* UART Register numbers                                                 */
#define UART_DATA 0
#define UART_IER  1
#define UART_IIR  2
#define UART_FCR  2
#define UART_LCR  3
#define UART_MCR  4
#define UART_LSR  5
#define UART_MSR  6

#define UART_DLL  0 /* LCR_DLAB = 1 */
#define UART_DLM  1 /* LCR_DLAB = 1 */

/* Alternative names for control and status registers                   */
#define rbr buffer              /**< receive buffer (read only)         */
#define thr buffer              /**< transmit hold (write only)         */
#define fcr iir                 /**< FIFO control (write only)          */
#define dll buffer              /**< divisor latch low byte             */
#define dlm ier                 /**< divisor latch high byte            */

/* UART Bit flags for control and status registers                      */
/* Interrupt enable bits                                                */
#define UART_IER_ERBFI  0x01    /**< Received data interrupt mask       */
#define UART_IER_ETBEI  0x02    /**< Transmitter buffer empty interrupt */
#define UART_IER_ELSI   0x04    /**< Recv line status interrupt mask    */
#define UART_IER_EMSI   0x08    /**< Modem status interrupt mask        */

/* Interrupt identification masks */
#define UART_IIR_IRQ    0x01    /**< Interrupt pending bit              */
#define UART_IIR_IDMASK 0x0E    /**< 3-bit field for interrupt ID       */
#define UART_IIR_MSC    0x00    /**< Modem status change                */
#define UART_IIR_THRE   0x02    /**< Transmitter holding register empty */
#define UART_IIR_RDA    0x04    /**< Receiver data available            */
#define UART_IIR_RLSI   0x06    /**< Receiver line status interrupt     */
#define UART_IIR_RTO    0x0C    /**< Receiver timed out                 */

/* FIFO control bits */
#define UART_FCR_EFIFO  0x01    /**< Enable in and out hardware FIFOs   */
#define UART_FCR_RRESET 0x02    /**< Reset receiver FIFO                */
#define UART_FCR_TRESET 0x04    /**< Reset transmit FIFO                */
#define UART_FCR_TRIG0  0x00    /**< RCVR FIFO trigger level one char   */
#define UART_FCR_TRIG1  0x40    /**< RCVR FIFO trigger level 1/4        */
#define UART_FCR_TRIG2  0x80    /**< RCVR FIFO trigger level 2/4        */
#define UART_FCR_TRIG3  0xC0    /**< RCVR FIFO trigger level 3/4        */

/* Line control bits */
#define UART_LCR_DLAB   0x80    /**< Divisor latch access bit           */
#define UART_LCR_8N1    0x03    /**< 8 bits, no parity, 1 stop          */

/* Modem control bits */
#define UART_MCR_OUT2   0x08    /**< User-defined OUT2.                 */
#define UART_MCR_LOOP   0x10    /**< Enable loopback test mode          */

/* Line status bits */
#define UART_LSR_DR     0x01    /**< Data ready                         */
#define UART_LSR_THRE   0x20    /**< Transmit-hold-register empty       */
#define UART_LSR_TEMT   0x40    /**< Transmitter empty                  */

#define UART_FIFO_LEN   16      /**< Size of the hardware FIFO buffer   */

/* UART Buffer lengths */
#define UART_IBLEN      1024
#define UART_OBLEN      1024

#endif /* _X86UART_H_ */
