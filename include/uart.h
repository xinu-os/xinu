/**
 * @file uart.h
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _UART_H_
#define _UART_H_

#include <device.h>
#include <semaphore.h>
#include <stddef.h>

/* UART Buffer lengths */
#ifndef UART_IBLEN
#define UART_IBLEN      1024
#endif
#ifndef UART_OBLEN
#define UART_OBLEN      1024
#endif

#define UART_BAUD       115200  /**< Default console baud rate.         */

/**
 * UART control block
 */
struct uart
{
    /* Pointers to associated structures */
    void *csr;                  /**< Control & status registers         */
    device *dev;                /**< Dev structure                      */

    /* Statistical Counts */
    uint cout;                  /**< Characters output                  */
    uint cin;                   /**< Characters input                   */
    uint lserr;                 /**< Receiver error count               */
    uint ovrrn;                 /**< Characters overrun                 */
    uint iirq;                  /**< Input IRQ count                    */
    uint oirq;                  /**< Output IRQ count                   */

    /* UART input fields */
    uchar iflags;               /**< Input flags                        */
    semaphore isema;            /**< Count of input bytes ready         */
    ushort istart;              /**< Index of first byte                */
    ushort icount;              /**< Bytes in buffer                    */
    uchar in[UART_IBLEN];       /**< Input buffer                       */

    /* UART output fields */
    uchar oflags;               /**< Output flags                       */
    semaphore osema;            /**< Count of buffer space free         */
    ushort ostart;              /**< Index of first byte                */
    ushort ocount;              /**< Bytes in buffer                    */
    uchar out[UART_OBLEN];      /**< Output buffer                      */
    volatile bool oidle;        /**< UART transmitter idle              */
};

extern struct uart uarttab[];

/* UART input flags */
#define UART_IFLAG_NOBLOCK    0x0001 /**< do non-blocking input         */
#define UART_IFLAG_ECHO       0x0002 /**< echo input                    */

/* UART output flags */
#define UART_OFLAG_NOBLOCK    0x0001 /**< do non-blocking output        */

/* uartControl() functions  */
#define UART_CTRL_SET_IFLAG   0x0010 /**< set input flags               */
#define UART_CTRL_CLR_IFLAG   0x0011 /**< clear input flags             */
#define UART_CTRL_GET_IFLAG   0x0012 /**< get input flags               */
#define UART_CTRL_SET_OFLAG   0x0013 /**< set output flags              */
#define UART_CTRL_CLR_OFLAG   0x0014 /**< clear output flags            */
#define UART_CTRL_GET_OFLAG   0x0015 /**< get output flags              */
#define UART_CTRL_OUTPUT_IDLE 0x0016 /**< determine if transmit idle    */

/* Driver functions */
devcall uartInit(device *);
devcall uartRead(device *, void *, uint);
devcall uartWrite(device *, const void *, uint);
devcall uartGetc(device *);
devcall uartPutc(device *, char);
devcall uartControl(device *, int, long, long);
interrupt uartInterrupt(void);
void uartStat(ushort);

/**
 * @ingroup uarthardware
 *
 * Initialize the UART hardware.
 */
devcall uartHwInit(device *);

/**
 * @ingroup uarthardware
 *
 * Immediately put a character to the UART.
 */
void uartHwPutc(void *, uchar);

/**
 * @ingroup uarthardware
 *
 * Print hardware-specific statistics about the UART.
 */
void uartHwStat(void *);

#endif                          /* _UART_H_ */
