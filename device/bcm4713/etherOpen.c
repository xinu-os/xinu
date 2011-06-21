/**
 * @file etherOpen.c
 * @provides etherOpen.
 *
 * $Id: etherOpen.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "bcm4713.h"
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
    struct bcm4713 *nicptr;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];
    nicptr = ethptr->csr;

    /* Request memory buffer pool for Rx DMA and intermediate buffers of
       all ethernet devices--both real and virtual */
    ethptr->inPool =
        bfpalloc(ETH_RX_BUF_SIZE + sizeof(struct ethPktBuffer),
                 ETH_RX_RING_ENTRIES + ETH_IBLEN * NETHER);
    ETHER_TRACE("eth%d inPool has been assigned pool ID %d.\r\n",
                devptr->minor, ethptr->inPool);
    if (SYSERR == ethptr->inPool)
    {
#ifdef DETAIL
        kprintf("eth%d inPool buffer error.\r\n", devptr->minor);
#endif                          /* DETAIL */
        return SYSERR;
    }

    /* Request memory buffer pool for Tx DMA */
    ethptr->outPool =
        bfpalloc(ETH_TX_BUF_SIZE + sizeof(struct ethPktBuffer),
                 ETH_TX_RING_ENTRIES);
    ETHER_TRACE("eth%d outPool has been assigned pool ID %d.\r\n",
                devptr->minor, ethptr->outPool);
    if (SYSERR == ethptr->outPool)
    {
#ifdef DETAIL
        kprintf("eth%d outPool buffer error.\r\n", devptr->minor);
#endif                          /* DETAIL */
        return SYSERR;
    }

    im = disable();

    /* Initialize DMA descriptor processor */
    nicptr->dmaTxControl = DMATX_CTRL_ENABLE;
    nicptr->dmaTxAddress = (ulong)ethptr->txRing & PMEM_MASK;

    nicptr->dmaRxControl = DMARX_CTRL_ENABLE |
        (ethptr->rxOffset << DMARX_CTRL_ROSHIFT);
    nicptr->dmaRxAddress = (ulong)ethptr->rxRing & PMEM_MASK;

    for (i = 0; i < ethptr->rxRingSize; i++)
    {
        allocRxBuffer(ethptr, i);
    }

    nicptr->dmaRxLast = ethptr->rxRingSize * sizeof(struct dmaDescriptor);
    ethptr->rxTail = ethptr->rxRingSize;        // Producer goes to tail.

    /* Configure NIC */
    nicptr->macControl |= MAC_CTRL_CRC32_ENAB;  /* enable crc32 */
    nicptr->rcvLazy = (1 << RCV_LAZY_FC_SHIFT); /* enable lazy Rx intr */

    etherControl(devptr, ETH_CTRL_SET_MAC, (long)ethptr->devAddress, 0);
    etherControl(devptr, ETH_CTRL_SET_LOOPBK, FALSE, 0);

    /* Set maximum incoming and outgoing packet length */
    nicptr->rxMaxLength = ETH_HEADER_LEN + ETH_VLAN_LEN
        + ethptr->mtu + ETH_CRC_LEN;
    nicptr->txMaxLength = ETH_HEADER_LEN + ETH_VLAN_LEN
        + ethptr->mtu + ETH_CRC_LEN;

    nicptr->txWatermark = TX_WATERMARK;

    nicptr->enetControl |= ENET_CTRL_ENABLE;

    ethptr->state = ETH_STATE_UP;
    nicptr->interruptMask = ethptr->interruptMask;

    restore(im);

    return OK;
}
