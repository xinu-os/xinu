/**
 * @file etherOpen.c
 * @provides etherOpen.
 *
 * $Id: etherOpen.c 2116 2009-11-03 20:55:05Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "ag71xx.h"
#include <ether.h>
#include <memory.h>
#include <bufpool.h>
#include <stdlib.h>
#include <interrupt.h>
#include <mips.h>

/**
 * Open an ethernet device for use.
 * @param  devptr ethernet device table entry
 * @return OK if opened properly, otherwise SYSERR
 */
devcall etherOpen(device *devptr)
{
    int i;
    irqmask im;
    struct ether *ethptr;
    struct ag71xx *nicptr;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];
    nicptr = ethptr->csr;

    im = disable();

    // Clear the Rx Counter.
    while (nicptr->rxStatus & RX_STAT_COUNT)
    {
        nicptr->rxStatus = RX_STAT_RECVD;
    }

    /* Initialize Tx ring */
    for (i = 0; i < ETH_TX_RING_ENTRIES; i++)
    {
        ethptr->txRing[i].next =
            (ulong)(ethptr->txRing + ((i + 1) % ETH_TX_RING_ENTRIES))
            & PMEM_MASK;
        ethptr->txRing[i].control = ETH_DESC_CTRL_EMPTY;
        ethptr->txBufs[i] = NULL;
    }
    /* Point NIC to start of Tx ring */
    nicptr->txDMA = ((ulong)ethptr->txRing) & PMEM_MASK;

    /* Allocate buffer pool for Tx DMA engine */
    ethptr->outPool =
        bfpalloc(ETH_TX_BUF_SIZE + ETH_PKT_RESERVE
                 + sizeof(struct ethPktBuffer), ETH_TX_RING_ENTRIES);
    if (SYSERR == ethptr->outPool)
    {
        ETH_TRACE("eth%d outPool buffer error.\r\n", devptr->minor);
        return SYSERR;
    }

    /* Allocate buffer pool for Rx DMA engine */
    ethptr->inPool =
        bfpalloc(ETH_RX_BUF_SIZE + ETH_PKT_RESERVE
                 + sizeof(struct ethPktBuffer),
                 ETH_RX_RING_ENTRIES + ETH_IBLEN);
    if (SYSERR == ethptr->inPool)
    {
        ETH_TRACE("eth%d inPool buffer error.\r\n", devptr->minor);
        return SYSERR;
    }

    /* Initialize Rx ring */
    for (i = 0; i < ETH_RX_RING_ENTRIES; i++)
    {
        ethptr->rxRing[i].next =
            (ulong)(ethptr->rxRing + ((i + 1) % ETH_RX_RING_ENTRIES))
            & PMEM_MASK;
    }

    /* Fill up Rx ring with available buffers */
    for (i = 0; i < ethptr->rxRingSize; i++)
    {
        allocRxBuffer(ethptr, i);
    }
    /* Point NIC to start of Tx ring */
    nicptr->rxDMA = ((ulong)ethptr->rxRing) & PMEM_MASK;

    // FIXME: Need to start up PHY here?
    // FIXME: Need to read MAC from NVRAM.
    etherControl(devptr, ETH_CTRL_SET_MAC, (long)ethptr->devAddress, 0);

    /* start Rx engine */
    nicptr->rxControl = RX_CTRL_RXE;

    ethptr->state = ETH_STATE_UP;
    /* enable interrupts */
    nicptr->interruptMask = ethptr->interruptMask;

    restore(im);

    return OK;
}
