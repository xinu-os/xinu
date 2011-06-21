/**
 * @file etherWrite.c
 * @provides etherWrite.
 *
 * $Id: etherWrite.c 2147 2009-12-19 07:23:03Z svn $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <bufpool.h>
#include "ag71xx.h"
#include <ether.h>
#include <interrupt.h>
#include <string.h>
#include <mips.h>
#include <network.h>

/**
 * Write a packet to the ethernet device
 * @param devptr device table entry
 * @param buf packet buffer
 * @param len size of the buffer
 * @return number of bytes sent from input buffer
 */
devcall etherWrite(device *devptr, void *buf, uint len)
{
    struct ether *ethptr = NULL;
    struct ag71xx *nicptr = NULL;
    struct ethPktBuffer *pkt = NULL;
    struct dmaDescriptor *dmaptr = NULL;
    irqmask im;
    ulong tail = 0;
    uchar *buffer = buf;
/* 	ulong *flushControl = (ulong *)0xB800007C; */

    ethptr = &ethertab[devptr->minor];
    nicptr = ethptr->csr;

    im = disable();
    if ((ETH_STATE_UP != ethptr->state)
        || (len < ETH_HEADER_LEN)
        || (len > (ETH_TX_BUF_SIZE - ETH_VLAN_LEN)))
    {
        restore(im);
        return SYSERR;
    }

    tail = ethptr->txTail % ETH_TX_RING_ENTRIES;
    dmaptr = &ethptr->txRing[tail];

    if (!(dmaptr->control & ETH_DESC_CTRL_EMPTY))
    {
        ETH_TRACE("dmaptr 0x%08X not empty.\r\n", dmaptr);
        ethptr->errors++;
        restore(im);
        return SYSERR;
    }

    pkt = (struct ethPktBuffer *)bufget(ethptr->outPool);
    if (SYSERR == (ulong)pkt)
    {
        ETH_TRACE("etherWrite() couldn't get a buffer!\r\n");
        ethptr->errors++;
        restore(im);
        return SYSERR;
    }

    /* Translate pkt pointer into uncached memory space */
    pkt = (struct ethPktBuffer *)((int)pkt | KSEG1_BASE);
    pkt->buf = (uchar *)(pkt + 1);
    pkt->data = pkt->buf;
    memcpy(pkt->data, buffer, len);

    /* Place filled buffer in outgoing queue */
    ethptr->txBufs[tail] = pkt;

    /* Add this buffer to the Tx ring. */
    /* Address on ring should be physical (USEG) for DMA engine */
    ethptr->txRing[tail].address = (ulong)pkt->data & PMEM_MASK;
    /* Clear empty flag and write the length */
    ethptr->txRing[tail].control = len & ETH_DESC_CTRL_LEN;

    ethptr->txTail++;

    if (nicptr->txStatus & TX_STAT_UNDER)
    {
        nicptr->txDMA = ((ulong)(ethptr->txRing + tail)) & PMEM_MASK;
        nicptr->txStatus = TX_STAT_UNDER;
    }
    nicptr->txControl = TX_CTRL_ENABLE;
    restore(im);

    return len;
}
