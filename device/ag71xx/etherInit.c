/**
 * @file etherInit.c
 *
 * Initialization for the Atheros ag71xx series of ethernet devices.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <mips.h>
#include "ag71xx.h"
#include <ether.h>
#include <memory.h>
#include <platform.h>
#include <clock.h>
#include <string.h>
#include <safemem.h>
#include <nvram.h>

/* Global table of ethernet devices */
struct ether ethertab[NETHER];

/* Implementation of etherInit() for the ag71xx; see the documentation for this
 * function in ether.h.  */
devcall etherInit(device *devptr)
{
    struct ether *ethptr;
    struct ag71xx *nicptr;
    uint *rstptr;
    uint rstbit;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];
    bzero(ethptr, sizeof(struct ether));
    ethptr->dev = devptr;
    ethptr->csr = devptr->csr;
    nicptr = (struct ag71xx *)devptr->csr;
    rstptr = (uint *)RESET_CORE;
    if (0 == devptr->minor)
    {
        rstbit = RESET_E0_MAC;
    }
    else
    {
        rstbit = RESET_E1_MAC;
    }

    ethptr->state = ETH_STATE_DOWN;
    ethptr->rxRingSize = ETH_RX_RING_ENTRIES;
    ethptr->txRingSize = ETH_TX_RING_ENTRIES;
    ethptr->mtu = ETH_MTU;
    ethptr->interruptMask = IRQ_TX_PKTSENT | IRQ_TX_BUSERR
        | IRQ_RX_PKTRECV | IRQ_RX_OVERFLOW | IRQ_RX_BUSERR;

    ethptr->errors = 0;
    ethptr->isema = semcreate(0);
    ethptr->istart = 0;
    ethptr->icount = 0;
    ethptr->ovrrun = 0;
    ethptr->rxOffset = ETH_PKT_RESERVE;

    /* Lookup canonical MAC in NVRAM, and store in ether struct */
    colon2mac(nvramGet("et0macaddr"), ethptr->devAddress);
    ethptr->addressLength = ETH_ADDR_LEN;

    // Reset the device.
    nicptr->macConfig1 |= MAC_CFG1_SOFTRESET;
    udelay(20);
    *rstptr |= rstbit;
    mdelay(100);
    *rstptr &= ~rstbit;
    mdelay(100);

    // Enable Tx and Rx.
    nicptr->macConfig1 = MAC_CFG1_TX | MAC_CFG1_SYNC_TX |
        MAC_CFG1_RX | MAC_CFG1_SYNC_RX;
    // Configure full duplex, auto padding CRC, and interface mode.
    nicptr->macConfig2 |=
        MAC_CFG2_FDX | MAC_CFG2_PAD | MAC_CFG2_LEN_CHECK |
        MAC_CFG2_IMNIBBLE;

    // Enable FIFO modules.
    nicptr->fifoConfig0 = FIFO_CFG0_WTMENREQ | FIFO_CFG0_SRFENREQ |
        FIFO_CFG0_FRFENREQ | FIFO_CFG0_STFENREQ | FIFO_CFG0_FTFENREQ;

    // FIXME
    // -> ag71xx_mii_ctrl_set_if(ag, pdata->mii_if);
    // Stores a '1' in 0x18070000 (MII mode)
    // Stores a '1' in 0x18070004 (RMII mode)

    // FRRD may be asserted only after the completion of the input frame.
    nicptr->fifoConfig1 = 0x0FFF0000;
    // Max out number of words to store in Rx RAM;
    nicptr->fifoConfig2 = 0x00001FFF;
    // Drop anything with errors in the Rx stats vector.
    nicptr->fifoConfig4 = 0x0003FFFF;
    // Drop short packets, set "don't care" on Rx stats vector bits.
    nicptr->fifoConfig5 = 0x0003FFFF;

    /* NOTE: because device initialization runs early in the system, */
    /*  we are assured that this stkget() call will return          */
    /*  page-aligned (and cache-aligned) boundaries.                */
    ethptr->rxBufs = stkget(PAGE_SIZE);
    ethptr->txBufs = stkget(PAGE_SIZE);
    ethptr->rxRing = stkget(PAGE_SIZE);
    ethptr->txRing = stkget(PAGE_SIZE);

    if ((SYSERR == (int)ethptr->rxBufs)
        || (SYSERR == (int)ethptr->txBufs)
        || (SYSERR == (int)ethptr->rxRing)
        || (SYSERR == (int)ethptr->txRing))
    {
#ifdef DETAIL
        kprintf("eth%d ring buffer allocation error.\r\n", devptr->minor);
#endif                          /* DETAIL */
        return SYSERR;
    }

    /* bump buffer pointers/rings to KSEG1 */
    ethptr->rxBufs =
        (struct ethPktBuffer
         **)(((ulong)ethptr->rxBufs - PAGE_SIZE +
              sizeof(int)) | KSEG1_BASE);
    ethptr->txBufs =
        (struct ethPktBuffer
         **)(((ulong)ethptr->txBufs - PAGE_SIZE +
              sizeof(int)) | KSEG1_BASE);
    ethptr->rxRing =
        (struct dmaDescriptor
         *)(((ulong)ethptr->rxRing - PAGE_SIZE +
             sizeof(int)) | KSEG1_BASE);
    ethptr->txRing =
        (struct dmaDescriptor
         *)(((ulong)ethptr->txRing - PAGE_SIZE +
             sizeof(int)) | KSEG1_BASE);

    /* Zero out the buffer pointers and rings */
    bzero(ethptr->rxBufs, PAGE_SIZE);
    bzero(ethptr->txBufs, PAGE_SIZE);
    bzero(ethptr->rxRing, PAGE_SIZE);
    bzero(ethptr->txRing, PAGE_SIZE);

    interruptVector[devptr->irq] = devptr->intr;
    enable_irq(devptr->irq);

    return OK;
}
