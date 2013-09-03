/**
 * @file     etherStat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <thread.h>
#include <device.h>
#include "ag71xx.h"
#include <ether.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tty.h>

#ifdef NETHER
void etherStat(ushort minor)
{
    struct ether *ethptr = NULL;        /* pointer to ether entry            */
    struct ag71xx *nicptr = NULL;       /* pointer to ether device registers */
    ulong tmp;
    uchar mac[6];

    /* Initialize pointers */
    ethptr = &ethertab[minor];
    nicptr = ethptr->csr;

    fprintf(stdout, "eth%d:\n", minor);
    control(ethptr->dev->num, ETH_CTRL_GET_MAC, (long)mac, 0);
    fprintf(stdout,
            "  MAC Address  %02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    fprintf(stdout, "      %8d MTU", ethptr->mtu);

    tmp = ethptr->state;
    fprintf(stdout, "  Device ");
    if (ETH_STATE_FREE == tmp)
    {
        fprintf(stdout, "FREE\n");
    }
    if (ETH_STATE_UP == tmp)
    {
        fprintf(stdout, "UP  \n");
    }
    if (ETH_STATE_DOWN == tmp)
    {
        fprintf(stdout, "DOWN\n");
    }

    fprintf(stdout, "  Errors %d\n", ethptr->errors);

    /* Ether statistics */
    tmp = ethptr->interruptStatus;
    fprintf(stdout, "  IRQ Status   0x%08X [", tmp);
    if (tmp & IRQ_TX_PKTSENT)
    {
        fprintf(stdout, " TX");
    }
    if (tmp & IRQ_TX_UNDERFLOW)
    {
        fprintf(stdout, " UF");
    }
    if (tmp & IRQ_TX_BUSERR)
    {
        fprintf(stdout, " TE");
    }
    if (tmp & IRQ_RX_PKTRECV)
    {
        fprintf(stdout, " RX");
    }
    if (tmp & IRQ_RX_BUSERR)
    {
        fprintf(stdout, " RE");
    }
    fprintf(stdout, " ]\n");

    tmp = ethptr->interruptMask;
    fprintf(stdout, "  IRQ Mask     0x%08X [", tmp);
    if (tmp & IRQ_TX_PKTSENT)
    {
        fprintf(stdout, " TX");
    }
    if (tmp & IRQ_TX_UNDERFLOW)
    {
        fprintf(stdout, " UF");
    }
    if (tmp & IRQ_TX_BUSERR)
    {
        fprintf(stdout, " TE");
    }
    if (tmp & IRQ_RX_PKTRECV)
    {
        fprintf(stdout, " RX");
    }
    if (tmp & IRQ_RX_BUSERR)
    {
        fprintf(stdout, " RE");
    }
    fprintf(stdout, " ]\n");

    tmp = nicptr->macConfig1;
    fprintf(stdout, "  MAC Config 1 0x%08X [", tmp);
    if (tmp & MAC_CFG1_TX)
    {
        fprintf(stdout, " TX");
    }
    if (tmp & MAC_CFG1_SYNC_TX)
    {
        fprintf(stdout, " ST");
    }
    if (tmp & MAC_CFG1_RX)
    {
        fprintf(stdout, " RX");
    }
    if (tmp & MAC_CFG1_SYNC_RX)
    {
        fprintf(stdout, " SR");
    }
    if (tmp & MAC_CFG1_LOOPBACK)
    {
        fprintf(stdout, " LB");
    }
    if (tmp & MAC_CFG1_SOFTRESET)
    {
        fprintf(stdout, " RS");
    }
    fprintf(stdout, " ]\n");

    tmp = nicptr->macConfig2;
    fprintf(stdout, "  MAC Config 2 0x%08X [", tmp);
    if (tmp & MAC_CFG2_FDX)
    {
        fprintf(stdout, " FDX");
    }
    if (tmp & MAC_CFG2_CRC)
    {
        fprintf(stdout, " CRC");
    }
    if (tmp & MAC_CFG2_PAD)
    {
        fprintf(stdout, " PAD");
    }
    if (tmp & MAC_CFG2_LEN_CHECK)
    {
        fprintf(stdout, " LEN");
    }
    fprintf(stdout, " ]\n");

    fprintf(stdout, "  FIFO Config0 0x%08X", nicptr->fifoConfig0);
    fprintf(stdout, "  FIFO Config1 0x%08X", nicptr->fifoConfig1);
    fprintf(stdout, "  FIFO Config2 0x%08X\n", nicptr->fifoConfig2);
    fprintf(stdout, "  FIFO Config3 0x%08X", nicptr->fifoConfig3);
    fprintf(stdout, "  FIFO Config4 0x%08X", nicptr->fifoConfig4);
    fprintf(stdout, "  FIFO Config5 0x%08X\n", nicptr->fifoConfig5);
    fprintf(stdout, "\n");

    fprintf(stdout, "  Tx Ring      0x%08X", ethptr->txRing);
    fprintf(stdout, "  Tx Bufs      0x%08X", ethptr->txBufs);
    fprintf(stdout, "  Tx Ring Size   %8d\n", ethptr->txRingSize);
    fprintf(stdout, "  Tx IRQ Count   %8d", ethptr->txirq);
    fprintf(stdout, "  Tx Head        %8d", ethptr->txHead);
    fprintf(stdout, "  Tx Tail        %8d\n", ethptr->txTail);
    fprintf(stdout, "  Tx Control   0x%08X", nicptr->txControl);
    fprintf(stdout, "  Tx DMA       0x%08X", nicptr->txDMA);
    fprintf(stdout, "  Tx Status    0x%08X\n", nicptr->txStatus);
    fprintf(stdout, "\n");
    fprintf(stdout, "  Rx Ring      0x%08X", ethptr->rxRing);
    fprintf(stdout, "  Rx Bufs      0x%08X", ethptr->rxBufs);
    fprintf(stdout, "  Rx Ring Size   %8d\n", ethptr->rxRingSize);
    fprintf(stdout, "  Rx IRQ Count   %8d", ethptr->rxirq);
    fprintf(stdout, "  Rx Head        %8d", ethptr->rxHead);
    fprintf(stdout, "  Rx Errors      %8d\n", ethptr->rxErrors);
    fprintf(stdout, "  Rx Control   0x%08X", nicptr->rxControl);
    fprintf(stdout, "  Rx DMA       0x%08X", nicptr->rxDMA);
    fprintf(stdout, "  Rx Status    0x%08X\n", nicptr->rxStatus);

    fprintf(stdout, "\n");
}

/**
 * @ingroup etherspecific
 */
void ethStat2(void)
{
    struct ether *ethptr = NULL;        /* pointer to ether entry            */
    int i = 0;

    /* Initialize pointers */
    ethptr = &ethertab[0];

    kprintf("ethStat2()\r\n");
    struct dmaDescriptor *dmaptr;
    for (i = 0; i < ETH_RX_RING_ENTRIES; i++)
    {
        dmaptr = &ethptr->rxRing[i];
        if (0 == dmaptr->control)
        {
            kprintf("[%08X]Rx Addr 0x%08X", (ulong)dmaptr,
                    dmaptr->address);
            kprintf("  Rx Cntl 0x%08X", dmaptr->control);
            kprintf("  Rx Next 0x%08X\r\n", dmaptr->next);
        }
    }
}

void etherThroughput(ushort minor)
{
    struct ether *ethptr = NULL;        /* pointer to ether entry            */
    struct ag71xx *nicptr = NULL;       /* pointer to ether device registers */

    /* Initialize pointers */
    ethptr = &ethertab[minor];
    nicptr = ethptr->csr;

    if (ethptr->dev != ethptr->phy)
    {
        return;
    }

    printf("Throughput monitoring not implemented for AG71xx Ethernet\n");
}

#endif
