/**
 * @file uartHwInit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>
#include <clock.h>
#include "pl011.h"

/* NOTE: the following definitions are probably BCM2835 specific */

/* Offset of UART registers from the starti of the GPIO registers. */
#define UART_GPIO_OFFSET 0x1000

#define GPFSEL1_OFFSET   0x4

/* Offset of GPPUD register from the start of the GPIO registers.  */
#define GPPUD_OFFSET     0x94

/* Offset of the GPPUDCLK0_OFFSET register from the start of the GPIO registers.
 * */
#define GPPUDCLK0_OFFSET 0x98

/* Set up the GPIO (general-purpose IO) pins needed to use the PL011 UART on the
 * Raspberry Pi.  */
static void setup_gpio_pins(void *uart_regs)
{
    void *gpio_regs = uart_regs - UART_GPIO_OFFSET;
    ulong sel;

    volatile ulong *GPFSEL1_ptr   = gpio_regs + GPFSEL1_OFFSET;
    volatile ulong *GPPUD_ptr     = gpio_regs + GPPUD_OFFSET;
    volatile ulong *GPPUDCLK0_ptr = gpio_regs + GPPUDCLK0_OFFSET;

    /* Select alternate function 0 on pins 14 and 15.  */
    sel = *GPFSEL1_ptr;
    sel &= ~(7 << 12);
    sel |= 4 << 12; /* Pin 14 */
    sel &= ~(7 << 15);
    sel |= 4 << 15; /* Pin 15 */
    *GPFSEL1_ptr = sel;

    /* Remove pull-up or pull-down on GPIO pins 14 and 15, which will be used
     * by the UART.  */
    *GPPUD_ptr = 0;
    udelay(2);
    *GPPUDCLK0_ptr = (1 << 14) | (1 << 15);
    udelay(2);
    *GPPUDCLK0_ptr = 0;
}

devcall uartHwInit(device *devptr)
{
    volatile struct pl011_uart_csreg *regptr = devptr->csr;

    /* TODO:  It doesn't work without this delay, but why? */
    udelay(1500);

    /* Disable the UART by zeroing the "control register".  */
    regptr->cr = 0;

    /* Configure the GPIO pins on the Raspberry Pi correctly. */
    setup_gpio_pins((void*)regptr);

    /* Poll the "flags register" to wait for the UART to stop transmitting or
     * receiving. */
    while (regptr->fr & PL011_FR_BUSY)
    {
    }

    /* Flush the transmit FIFO by marking FIFOs as disabled in the "line control
     * register". */
    regptr->lcrh &= ~PL011_LCRH_FEN;

    /* Clear pending interrupts by writing to the "interrupt clear" register.
     * Note: In the documentation for the BCM2835 SoC, several bits are marked
     * as "Unsupported; write zero, read as don't care."  These are commented
     * out below. */
    regptr->icr = (PL011_ICR_OEIC |
                   PL011_ICR_BEIC |
                   PL011_ICR_PEIC |
                   PL011_ICR_FEIC |
                   PL011_ICR_RTIC |
                   PL011_ICR_TXIC |
                   PL011_ICR_RXIC |
                   0 /* PL011_ICR_DSRMIC */ |
                   0 /* PL011_ICR_DCDMIC */ |
                   PL011_ICR_CTSMIC |
                   0 /* PL011_ICR_RIMIC */);

    /* Set the UART's baud rate.  This is done by writing to separate "integer
     * baud rate divisor" and "fractional baud rate divisor" registers.  */
    regptr->ibrd = PL011_BAUD_INT(115200);
    regptr->fbrd = PL011_BAUD_FRAC(115200);

    /* Write the appropriate values to the UART's "line control register" to set
     * it to an 8 bit word length, with parity bit and FIFOs disabled.  */
    regptr->lcrh = PL011_LCRH_WLEN_8BIT;

    /* Allow the UART to generate interrupts only when receiving or
     * transmitting.  */
    regptr->imsc = PL011_IMSC_RXIM | PL011_IMSC_TXIM;

    /* We have decided that we are going to leave FIFOs off for now.  Since the
     * FIFOs are of size 16 and the lowest trigger level you can set for the
     * receive FIFO is 1/8, the first interrupt isn't triggered until there are
     * at least two bytes in the receive FIFO.  We want an interrupt as soon as
     * a byte arrives */
#if 0
    /* Enable UART FIFOs. */
    regptr->lcrh |= PL011_LCRH_FEN;

    /* Set the interrupt FIFO level select register.  This configures the amount
     * that the receive or transmit FIFOs can fill up before an interrupt is
     * generated.  */
    regptr->ifls = PL011_IFLS_RXIFLSEL_SEVENEIGHTHS | PL011_IFLS_TXIFLSEL_EIGHTH;
#endif

    /* Enable the UART, with both the receive and transmit functionality, by
     * writing to its control register.  */
    regptr->cr = PL011_CR_RXE | PL011_CR_TXE | PL011_CR_UARTEN;

    /* Register the UART's interrupt handler with XINU's interrupt vector, then
     * actually enable the UART's interrupt line.  */
    interruptVector[devptr->irq] = devptr->intr;
    enable_irq(devptr->irq);
    return OK;
}
