/**
 * @file allocRxBuffer.c
 * @provides allocRxBuffer.
 *
 * $Id: allocRxBuffer.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <bufpool.h>
#include <mips.h>

/**
 * Allocate an ethernet packet buffer structure
 * @param ethptr ethernet table entry
 * @param destIndex destination index in ethernet reciever ring
 * @return bytes allocated
 */
int allocRxBuffer(struct ether *ethptr, int destIndex)
{
    struct ethPktBuffer *pkt = NULL;
    struct dmaDescriptor *dmaptr = NULL;

    destIndex %= ethptr->rxRingSize;
    pkt = bufget(ethptr->inPool);
    if (SYSERR == (ulong)pkt)
    {
#ifdef DETAIL
        kprintf("eth0 allocRxBuffer() error\r\n");
#endif                          /* DETAIL */
        return SYSERR;
    }
    pkt->length = ETH_RX_BUF_SIZE;
    pkt->buf = (uchar *)(pkt + 1);

    /* Data region offset by size of rx header. */
    pkt->data = pkt->buf + ethptr->rxOffset;

    ethptr->rxBufs[destIndex] = pkt;

    /* Fill in DMA descriptor fields */
    dmaptr = ethptr->rxRing + destIndex;
    dmaptr->control = ETH_DESC_CTRL_LEN & pkt->length;
    if ((ethptr->rxRingSize - 1) == destIndex)
    {
        dmaptr->control |= ETH_DESC_CTRL_EOT;
    }
    dmaptr->address = (ulong)(pkt->buf) & PMEM_MASK;

#ifdef DETAIL
    kprintf("eth0 rxBufs[%d] = 0x%08X\r\n",
            destIndex, ethptr->rxBufs[destIndex]);

    kprintf
        ("  control 0x%08X address 0x%08X buffer  0x%08X\r\n",
         dmaptr->control, dmaptr->address, pkt);

    if (pkt)
    {
        kprintf
            ("  epb buf 0x%08X epb dat 0x%08X epb len   %8d\r\n",
             pkt->buf, pkt->data, pkt->length);
        struct rxHeader *rh;

        rh = (struct rxHeader *)pkt->buf;
        kprintf
            ("  rxHead  0x%08X length  0x%08X flags   0x%08X\r\n",
             rh, rh ? rh->length : 0, rh ? rh->flags : 0);
    }
#endif                          /* DETAIL */

    return OK;
}
