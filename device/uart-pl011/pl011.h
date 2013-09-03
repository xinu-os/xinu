/**
 * @file pl011.h
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _PL011_H_
#define _PL011_H_

/**
 * Control and status registers for the PL011 UART.  This structure is
 * mapped directly to the base address for the CSR.
 */
struct pl011_uart_csreg
{
    volatile unsigned int dr;          /**< Data Register                        */ //0x00
    volatile unsigned int rsrecr;      /**< Receive status/error clear register  */ //0x04
    volatile unsigned int dont_use_a;  /** spacer                                */ //0x08
    volatile unsigned int dont_use_b;  /** spacer                                */ //0x0C
    volatile unsigned int dont_use_c;  /** spacer                                */ //0x10
    volatile unsigned int dont_use_d;  /** spacer                                */ //0x14
    volatile unsigned int fr;          /**< Flag Register                        */ //0x18
    volatile unsigned int dont_use_e;  /** spacer                                */ //0x1C
    volatile unsigned int ilpr;        /**< Not in use                           */ //0x20
    volatile unsigned int ibrd;        /**< Integer baud rate divisor            */ //0x24
    volatile unsigned int fbrd;        /**< Fractional baud rate divisor         */ //0x28
    volatile unsigned int lcrh;        /**< Line Control Register                */ //0x2C
    volatile unsigned int cr;          /**< Control Register                     */ //0x30
    volatile unsigned int ifls;        /**< Interupt FIFO level select register  */ //0x34
    volatile unsigned int imsc;        /**< Interupt Mask Set Clear Register     */ //0x38
    volatile unsigned int ris;         /**< Raw Interupt Status Register         */ //0x3C
    volatile unsigned int mis;         /**< Masked Interupt Status Register      */ //0x40
    volatile unsigned int icr;         /**< Interupt Clear Register              */ //0x44
};

/* UART Bit flags for control and status registers                      */
#define PL011_DR_OE (1<<11) //set to 1 on overrun error
#define PL011_DR_BE (1<<10) //set to 1 on break condition
#define PL011_DR_PE (1<<9)  //set to 1 on parity error
#define PL011_DR_FE (1<<8)  //set to 1 on framing error

#define PL011_RSRECR_OE (1<<3) //set to 1 on overrun error
#define PL011_RSRECR_BE (1<<2) //set to 1 on break condition
#define PL011_RSRECR_PE (1<<1) //set to 1 on parity error
#define PL011_RSRECR_FE (1<<0) //set to 1 on framing error

#define PL011_FR_RI   (1<<8) //set to 1 when ring indicator pin is low
#define PL011_FR_TXFE (1<<7) //set to 1 when TX FIFO/register is empty
#define PL011_FR_RXFF (1<<6) //set to 1 when RX FIFO/register is full
#define PL011_FR_TXFF (1<<5) //set to 1 when TX FIFO/register is full
#define PL011_FR_RXFE (1<<4) //set to 1 when RX FIFO/register is empty
#define PL011_FR_BUSY (1<<3) //set to 1 when UART is transmitting data
#define PL011_FR_DCD  (1<<2) //set to 1 when DCD pin is low
#define PL011_FR_DSR  (1<<1) //set to 1 when DSR pin is low
#define PL011_FR_CTS  (1<<0) //set to 1 when CTS pin is low

#define PL011_LCRH_SPS  (1<<7) //
//word length
#define PL011_LCRH_WLEN_8BIT (0b11<<5)
#define PL011_LCRH_WLEN_7BIT (0b10<<5)
#define PL011_LCRH_WLEN_6BIT (0b01<<5)
#define PL011_LCRH_WLEN_5BIT (0b00<<5)
#define PL011_LCRH_FEN  (1<<4) //set to 1 to enable TX and RX FIFOs
#define PL011_LCRH_STP2 (1<<3) //set to 1 to send 2 stop bits
#define PL011_LCRH_EPS  (1<<2) //set to 1 for even parity, 0 for odd
#define PL011_LCRH_PEN  (1<<1) //set to 1 to enable parity
#define PL011_LCRH_BRK  (1<<0) //set to 1 to force the TX pin low

#define PL011_CR_CTSEN  (1<<15) //set to 1 to enable CTS hardware flow control
#define PL011_CR_RTSEN  (1<<14) //set to 1 to enable RTS hardware flow control
#define PL011_CR_OUT2   (1<<13) //set to 1 to set out2 to 0
#define PL011_CR_OUT1   (1<<12) //set to 1 to set out1 to 0
#define PL011_CR_RTS    (1<<11) //set to 1 to set the RTS pin low
#define PL011_CR_DTR    (1<<10) //set to 1 to set the DTR pin low
#define PL011_CR_RXE    (1<<9)  //set to 1 to enable receiving
#define PL011_CR_TXE    (1<<8)  //set to 1 to enable transmitting
#define PL011_CR_LBE    (1<<7)  //set to 1 to enable loopback
#define PL011_CR_SIRLP  (1<<2)  //sets SIR IrDA mode (unused?)
#define PL011_CR_SIREN  (1<<1)  //enables SIR IrDA mode (unused?)
#define PL011_CR_UARTEN (1<<0)  //set to 1 to enable the UART

//RX FIFO fill interrupt threshold
#define PL011_IFLS_RXIFLSEL_EIGHTH        (0b000<<3)
#define PL011_IFLS_RXIFLSEL_QUARTER       (0b001<<3)
#define PL011_IFLS_RXIFLSEL_HALF          (0b010<<3)
#define PL011_IFLS_RXIFLSEL_THREEQUARTERS (0b011<<3)
#define PL011_IFLS_RXIFLSEL_SEVENEIGHTHS  (0b100<<3)
//TX FIFO fill interrupt threshold
#define PL011_IFLS_TXIFLSEL_EIGHTH        (0b000<<0)
#define PL011_IFLS_TXIFLSEL_QUARTER       (0b001<<0)
#define PL011_IFLS_TXIFLSEL_HALF          (0b010<<0)
#define PL011_IFLS_TXIFLSEL_THREEQUARTERS (0b011<<0)
#define PL011_IFLS_TXIFLSEL_SEVENEIGHTHS  (0b100<<0)

#define PL011_IMSC_OEIM   (1<<10) //overrun error mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_BEIN   (1<<9)  //break error mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_PEIM   (1<<8)  //parity error mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_FEIM   (1<<7)  //framing error mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_RTIM   (1<<6)  //receive timeout mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_TXIM   (1<<5)  //transmit interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_RXIM   (1<<4)  //receive interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_DSRMIM (1<<3)  //DSR interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_DCDMIM (1<<2)  //DCD interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_CTSMIM (1<<1)  //CTS interrupt mask bit, if 1: this interrupt is enabled
#define PL011_IMSC_RIMIM  (1<<0)  //RI interrupt mask bit, if 1: this interrupt is enabled

#define PL011_RIS_OERIS   (1<<10) //overrun error interrupt status
#define PL011_RIS_BERIS   (1<<9)  //break error interrupt status
#define PL011_RIS_PERIS   (1<<8)  //parity error interrupt status
#define PL011_RIS_FERIS   (1<<7)  //framing error interrupt status
#define PL011_RIS_RTRIS   (1<<6)  //receive timeout interrupt status
#define PL011_RIS_TXRIS   (1<<5)  //transmit interrupt status
#define PL011_RIS_RXRIS   (1<<4)  //receive interrupt status
#define PL011_RIS_DSRRMIS (1<<3)  //DSR interrupt status
#define PL011_RIS_DCDRMIS (1<<2)  //DCD interrupt status
#define PL011_RIS_CTSRMIS (1<<1)  //CTS interrupt status
#define PL011_RIS_RIRMIS  (1<<0)  //RI interrupt status

//read only
#define PL011_MIS_OEMIS   (1<<10) //overrun error interrupt status
#define PL011_MIS_BEMIS   (1<<9)  //break error interrupt status
#define PL011_MIS_PEMIS   (1<<8)  //parity error interrupt status
#define PL011_MIS_FEMIS   (1<<7)  //framing error interrupt status
#define PL011_MIS_RTMIS   (1<<6)  //receive timeout interrupt status
#define PL011_MIS_TXMIS   (1<<5)  //transmit interrupt status
#define PL011_MIS_RXMIS   (1<<4)  //receive interrupt status
#define PL011_MIS_DSRMMIS (1<<3)  //DSR interrupt status
#define PL011_MIS_DCDMMIS (1<<2)  //DCD interrupt status
#define PL011_MIS_CTSMMIS (1<<1)  //CTS interrupt status
#define PL011_MIS_RIMMIS  (1<<0)  //RI interrupt status

#define PL011_ICR_OEIC   (1<<10) //overrun error interrupt clear
#define PL011_ICR_BEIC   (1<<9)  //break error interrupt clear
#define PL011_ICR_PEIC   (1<<8)  //parity error interrupt clear
#define PL011_ICR_FEIC   (1<<7)  //framing error interrupt clear
#define PL011_ICR_RTIC   (1<<6)  //receive timeout interrupt clear
#define PL011_ICR_TXIC   (1<<5)  //transmit interrupt clear
#define PL011_ICR_RXIC   (1<<4)  //receive interrupt clear
#define PL011_ICR_DSRMIC (1<<3)  //DSR interrupt clear
#define PL011_ICR_DCDMIC (1<<2)  //DCD interrupt clear
#define PL011_ICR_CTSMIC (1<<1)  //CTS interrupt clear
#define PL011_ICR_RIMIC  (1<<0)  //RI interrupt clear

#define PL011_FIFO_LEN   8       /**< During testing on a Raspberry Pi, I could only send 8. */

#define PL011_BAUD_INT(x) (3000000 / (16 * (x)))
#define PL011_BAUD_FRAC(x) (int)((((3000000.0 / (16.0 * (x)))-PL011_BAUD_INT(x))*64.0)+0.5) //9600 baud may be slightly off with this calcualtion

#endif                          /* _PL011_H_ */
