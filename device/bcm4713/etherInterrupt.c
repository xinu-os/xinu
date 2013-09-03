/**
 * @file etherInterrupt.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <stdlib.h>
#include <mips.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <vlan.h>
#include <platform.h>
#include <string.h>
#include <bufpool.h>
#include <network.h>

extern int resdefer;

/**
 * @ingroup etherspecific
 */
void rxPackets(struct ether *ethptr, struct bcm4713 *nicptr)
{
    ulong head = 0, tail = 0;
    struct ethPktBuffer *pkt = NULL;
    struct rxHeader *rh = NULL;
    struct vlanPkt *lanptr = NULL;
    struct ether *phyptr = 0;

    /* 12-bit descriptor indicates where card is currently placing   */
    /*  received packets into the ring.                              */
    tail = (nicptr->dmaRxStatus & DMARX_STAT_CDMASK)
        / sizeof(struct dmaDescriptor);
    /* rxHead indicates where we last left off pulling received      */
    /*  packets off of the ring.                                     */
    head = ethptr->rxHead;
    while (head != tail)
    {
        pkt = ethptr->rxBufs[head];
        rh = (struct rxHeader *)pkt->buf;
        lanptr = (struct vlanPkt *)pkt->data;

        if (ETH_TYPE_VLAN == net2hs(lanptr->tpi))
        {
            phyptr = &ethertab[net2hs(lanptr->vlanId) & ETH_VLAN_IDMASK];
        }
        else
        {
            phyptr = ethptr;
        }

        if ((rh->length >
             ETH_MAX_PKT_LEN + ETH_CRC_LEN)
            || (rh->flags & ETH_RX_FLAG_ERRORS))
        {
            phyptr->rxErrors++;
            bzero(pkt->buf, pkt->length);
        }
        else
        {
            if (phyptr->icount < ETH_IBLEN)
            {
                allocRxBuffer(ethptr, head);
                phyptr->in[(phyptr->istart + phyptr->icount) %
                           ETH_IBLEN] = pkt;
                phyptr->icount++;
                signaln(phyptr->isema, 1);
            }
            else
            {
                phyptr->ovrrun++;
                bzero(pkt->buf, pkt->length);
            }
        }
        ethptr->rxTail = (ethptr->rxTail + 1) % ethptr->rxRingSize;
        nicptr->dmaRxLast = ethptr->rxTail * sizeof(struct dmaDescriptor);
        head = (head + 1) % ethptr->rxRingSize;
    }
    ethptr->rxHead = head;
}

/**
 * @ingroup etherspecific
 */
void txPackets(struct ether *ethptr, struct bcm4713 *nicptr)
{
    struct ethPktBuffer **epb = NULL;
    struct ethPktBuffer *pkt = NULL;
    ulong current = 0, head = 0;

    current = (nicptr->dmaTxStatus & DMATX_STAT_CDMASK)
        / sizeof(struct dmaDescriptor);

    for (head = ethptr->txHead; head != current;
         head = (head + 1) % ethptr->txRingSize)
    {
        epb = &ethptr->txBufs[head];
        pkt = *epb;
        if (NULL == pkt)
        {
            continue;
        }
        buffree((void *)((ulong)pkt & (PMEM_MASK | KSEG0_BASE)));
        *epb = NULL;
    }
    ethptr->txHead = head;
    nicptr->gpTimer = 0;
}

/**
 * @ingroup etherspecific
 *
 * Decode and handle hardware interrupt request from ethernet device.
 */
interrupt etherInterrupt(void)
{
    int ethnum;
    struct ether *ethptr;
    struct bcm4713 *nicptr;
    uint status, mask;

    /* Initialize structure pointers */
    ethptr = &ethertab[0];      /* default physical ethernet for WRT54GL */
    if (!ethptr)
    {
        return;
    }
    nicptr = ethptr->csr;
    if (!nicptr)
    {
        return;
    }

    mask = nicptr->interruptMask;
    status = nicptr->interruptStatus & mask;

    /* remember interrupt status in ether struct */
    ethptr->interruptStatus = status;

    if (!status)
    {
        return;
    }

    resdefer = 1;               /* defer rescheduling */

    if (status & ISTAT_TX)
    {
        ethptr->txirq++;
        txPackets(ethptr, nicptr);
        /* Set Rx timeout to 2 seconds after last Tx */
        nicptr->gpTimer = 2 * platform.clkfreq;
    }

    if (status & ISTAT_RX)
    {
        ethptr->rxirq++;
        rxPackets(ethptr, nicptr);
        /* Set Rx timeout to 0 */
        nicptr->gpTimer = 0;
    }

    if (status & ISTAT_ERRORS)
    {
        /* Error closes physical NIC as well as vlans */
        for (ethnum = 0; ethnum < NETHER; ethnum++)
        {
            ethptr = &ethertab[ethnum];
            if (!ethptr)
            {
                continue;
            }
            ethptr->errors++;
            etherClose(ethptr->dev);
        }
    }

    /* signal the card with the interrupts we handled */
    nicptr->interruptStatus = status;

    if (--resdefer > 0)
    {
        resdefer = 0;
        resched();
    }

    return;
}
