/**
 * @file etherInterrupt.c
 * @provides etherInterrupt.
 *
 * $Id: etherInterrupt.c 2128 2009-11-17 01:38:29Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <thread.h>
#include <stdlib.h>
#include <mips.h>
#include <device.h>
#include "ag71xx.h"
#include <ether.h>
#include <vlan.h>
#include <platform.h>
#include <string.h>
#include <bufpool.h>
#include <network.h>

extern int resdefer;

/**
 * Receive packet interrupt handler.
 */
void rxPackets(struct ether *ethptr, struct ag71xx *nicptr)
{
    struct dmaDescriptor *dmaptr;
    struct ethPktBuffer *pkt = NULL;
    int head = 0;

    while (1)
    {
        head = ethptr->rxHead % ETH_RX_RING_ENTRIES;
        dmaptr = &ethptr->rxRing[head];
        if (dmaptr->control & ETH_DESC_CTRL_EMPTY)
        {
            nicptr->rxStatus = RX_STAT_RECVD;
            break;
        }

        pkt = ethptr->rxBufs[head];
        pkt->length = dmaptr->control & ETH_DESC_CTRL_LEN;

        if (ethptr->icount < ETH_IBLEN)
        {
            allocRxBuffer(ethptr, head);
            ethptr->in[(ethptr->istart + ethptr->icount) % ETH_IBLEN] =
                pkt;
            ethptr->icount++;
            signaln(ethptr->isema, 1);
        }
        else
        {
            ethptr->ovrrun++;
            bzero(pkt->buf, pkt->length);
        }

        ethptr->rxHead++;
        // Clear Rx interrupt.
        nicptr->rxStatus = RX_STAT_RECVD;
        // FIXME
        break;
    }
}

/**
 * Transmit packet interrupt handler
 */
void txPackets(struct ether *ethptr, struct ag71xx *nicptr)
{
    struct dmaDescriptor *dmaptr;
    struct ethPktBuffer **epb = NULL;
    struct ethPktBuffer *pkt = NULL;
    ulong head = 0;

    // kprintf("txS=0x%08X\r\n", nicptr->txStatus);
    if (ethptr->txHead == ethptr->txTail)
    {
        nicptr->txStatus = TX_STAT_SENT;
        return;
    }

    while (ethptr->txHead != ethptr->txTail)
    {
        head = ethptr->txHead % ETH_TX_RING_ENTRIES;
        dmaptr = &ethptr->txRing[head];
        if (!(dmaptr->control & ETH_DESC_CTRL_EMPTY))
        {
            break;
        }

        epb = &ethptr->txBufs[head];

        // Clear Tx interrupt.
        nicptr->txStatus = TX_STAT_SENT;

        ethptr->txHead++;
        pkt = *epb;
        if (NULL == pkt)
        {
            continue;
        }
        buffree((void *)((ulong)pkt & (PMEM_MASK | KSEG0_BASE)));
        *epb = NULL;
    }
}

/**
 * Decode and handle hardware interrupt request from ethernet device.
 */
interrupt etherInterrupt(void)
{
//    int ethnum;
    struct ether *ethptr;
    struct ag71xx *nicptr;
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

    if (status & IRQ_TX_PKTSENT)
    {
        ethptr->txirq++;
        txPackets(ethptr, nicptr);
    }

    if (status & IRQ_RX_PKTRECV)
    {
        ethptr->rxirq++;
        rxPackets(ethptr, nicptr);
    }

    if (status & IRQ_RX_OVERFLOW)
    {
        // Clear interrupt, restart processing.
        nicptr->rxStatus = RX_STAT_OVERFLOW;
        nicptr->rxControl = RX_CTRL_RXE;
        ethptr->errors++;
    }

    if ((status & IRQ_TX_UNDERFLOW) ||
        (status & IRQ_TX_BUSERR) || (status & IRQ_RX_BUSERR))
    {
        kprintf("Ethernet ERROR 0x%08X\r\n", status);
        while (1)
            ;
        ethptr->errors++;
        // etherClose(ethptr->dev);
    }

    if (--resdefer > 0)
    {
        resdefer = 0;
        resched();
    }

    return;
}
