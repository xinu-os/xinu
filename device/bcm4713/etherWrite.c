/**
 * @file etherWrite.c
 * @provides etherWrite.
 *
 * $Id: etherWrite.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <vlan.h>
#include <bufpool.h>
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
    struct ether *ethptr;
    struct bcm4713 *nicptr;
    struct ethPktBuffer *pkt = NULL;
    struct vlanPkt *lanptr;
    struct ether *phyptr;
    irqmask im;
    ulong entry = 0, control = 0;
    uchar *buffer = buf;
    uint outlen;

    ethptr = &ethertab[devptr->minor];
    nicptr = ethptr->csr;

    if (ETH_STATE_UP != ethptr->state)
    {
        return SYSERR;
    }
    phyptr = &ethertab[ethptr->phy->minor];
    if (ETH_STATE_UP != phyptr->state)
    {
        return SYSERR;
    }

    /* make sure packet is not too small */
    if (len < ETH_HEADER_LEN)
    {
        return SYSERR;
    }

    /* make sure packet is not too big */
    if (len > (ETH_TX_BUF_SIZE - ETH_VLAN_LEN))
    {
        return SYSERR;
    }

    pkt = (struct ethPktBuffer *)bufget(phyptr->outPool);
    if (SYSERR == (ulong)pkt)
    {
        return SYSERR;
    }

    /* set outbound packet length to currently buffer length */
    outlen = len;

    pkt = (struct ethPktBuffer *)((int)pkt | KSEG1_BASE);
    pkt->buf = (uchar *)(pkt + 1);
    pkt->data = pkt->buf;
    lanptr = (struct vlanPkt *)pkt->data;

    /* Copy packet to DMA buffer with added vlan tag */
    memcpy(pkt->data, buffer, 12);
    lanptr->tpi = hs2net(ETH_TYPE_VLAN);
    lanptr->vlanId = hs2net(devptr->minor);
    memcpy(pkt->data + 16, buffer + 12, outlen - 12);
    outlen += ETH_VLAN_LEN;     /* account for vlan tag addition */
    pkt->length = outlen;

    /* Place filled buffer in outgoing queue */
    im = disable();
    entry = phyptr->txTail;
    phyptr->txBufs[entry] = pkt;

    control = outlen & ETH_DESC_CTRL_LEN;
    /* Mark as start and end of frame, interrupt on completion. */
    control |= ETH_DESC_CTRL_IOC | ETH_DESC_CTRL_SOF | ETH_DESC_CTRL_EOF;
    if (phyptr->txRingSize - 1 == entry)
    {
        control |= ETH_DESC_CTRL_EOT;
    }

    /* Add this buffer to the Tx ring. */
    phyptr->txRing[entry].control = control;
    phyptr->txRing[entry].address = (ulong)pkt->data & PMEM_MASK;

    phyptr->txTail = (entry + 1) % phyptr->txRingSize;
    entry = phyptr->txTail;

    nicptr->dmaTxLast = entry * sizeof(struct dmaDescriptor);

    restore(im);

    return len;
}
