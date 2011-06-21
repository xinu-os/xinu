/**
 * @file     etherStat.c
 * @provides etherStat.
 *
 * $Id: etherStat.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void vethStat(ushort);

void etherStat(ushort minor)
{
    struct ether *ethptr = NULL;        /* pointer to ether entry            */
    struct bcm4713 *nicptr = NULL;      /* pointer to ether device registers */
    ulong tmp;
    uchar mac[6];

    /* Initialize pointers */
    ethptr = &ethertab[minor];
    nicptr = ethptr->csr;

    printf("%s:\n", (ethptr->dev)->name);
    control(ethptr->dev->num, ETH_CTRL_GET_MAC, (long)mac, 0);
    printf("  MAC Address  %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);

    if (ethptr->dev != ethptr->phy)
    {
        vethStat(minor);
        return;
    }

    printf("  %8d MTU\n", ethptr->mtu);

    tmp = ethptr->state;
    printf("  Device ");
    if (ETH_STATE_FREE == tmp)
    {
        printf("FREE");
    }
    if (ETH_STATE_UP == tmp)
    {
        printf("UP  ");
    }
    if (ETH_STATE_DOWN == tmp)
    {
        printf("DOWN");
    }

    printf("  RxErrors %d           ", ethptr->rxErrors);
    printf("  Errors %d\n", ethptr->errors);

    /* Ether statistics */
    tmp = ethptr->interruptStatus;
    printf("  IRQ Status   0x%08X [", tmp);
    if (tmp & ISTAT_PME)
    {
        printf(" PME");
    }
    if (tmp & ISTAT_TO)
    {
        printf(" TO");
    }
    if (tmp & ISTAT_DSCE)
    {
        printf(" DSCE");
    }
    if (tmp & ISTAT_DATAE)
    {
        printf(" DATAE");
    }
    if (tmp & ISTAT_DPE)
    {
        printf(" DPE");
    }
    if (tmp & ISTAT_RDU)
    {
        printf(" RDU");
    }
    if (tmp & ISTAT_RFO)
    {
        printf(" RFO");
    }
    if (tmp & ISTAT_TFU)
    {
        printf(" TFU");
    }
    if (tmp & ISTAT_RX)
    {
        printf(" RX");
    }
    if (tmp & ISTAT_TX)
    {
        printf(" TX");
    }
    if (tmp & ISTAT_EMAC)
    {
        printf(" EMAC");
    }
    if (tmp & ISTAT_MII_WRITE)
    {
        printf(" MII_WRITE");
    }
    if (tmp & ISTAT_MII_READ)
    {
        printf(" MII_READ");
    }
    printf(" ]\n");

    tmp = ethptr->interruptMask;
    printf("  IRQ Mask     0x%08X [", tmp);
    if (tmp & ISTAT_PME)
    {
        printf(" PME");
    }
    if (tmp & ISTAT_TO)
    {
        printf(" TO");
    }
    if (tmp & ISTAT_DSCE)
    {
        printf(" DSCE");
    }
    if (tmp & ISTAT_DATAE)
    {
        printf(" DATAE");
    }
    if (tmp & ISTAT_DPE)
    {
        printf(" DPE");
    }
    if (tmp & ISTAT_RDU)
    {
        printf(" RDU");
    }
    if (tmp & ISTAT_RFO)
    {
        printf(" RFO");
    }
    if (tmp & ISTAT_TFU)
    {
        printf(" TFU");
    }
    if (tmp & ISTAT_RX)
    {
        printf(" RX");
    }
    if (tmp & ISTAT_TX)
    {
        printf(" TX");
    }
    if (tmp & ISTAT_EMAC)
    {
        printf(" EMAC");
    }
    if (tmp & ISTAT_MII_WRITE)
    {
        printf(" MII_WRITE");
    }
    if (tmp & ISTAT_MII_READ)
    {
        printf(" MII_READ");
    }
    printf(" ]\n");

    printf("  Tx IRQ Count   %8d", ethptr->txirq);
    printf("  Tx Octets      %8d", nicptr->txGoodOctets);
    printf("  Tx Packets     %8d\n", nicptr->txGoodPackets);
    printf("  Tx < 65 octets %8d", nicptr->tx_64);
    printf("  Tx < 128       %8d", nicptr->tx_65_127);
    printf("  Tx < 256       %8d\n", nicptr->tx_128_255);
    printf("  Tx < 512       %8d", nicptr->tx_256_511);
    printf("  Tx < 1024      %8d", nicptr->tx_512_1023);
    printf("  Tx >= 1024     %8d\n", nicptr->tx_1024_max);
    printf("  Tx Broadcast   %8d", nicptr->txBroadcast);
    printf("  Tx Multicast   %8d", nicptr->txMulticast);
    printf("  Tx Jabber      %8d\n", nicptr->txJabber);
    printf("  Tx Oversize    %8d", nicptr->txOversize);
    printf("  Tx Fragment    %8d", nicptr->txFragment);
    printf("  Tx Underruns   %8d\n", nicptr->txUnderruns);
    printf("  Tx Collisions  %8d", nicptr->txCollisions);
    printf("  Tx Status    0x%08X\n", nicptr->dmaTxStatus);
    printf("\n");
    printf("  Rx IRQ Count   %8d", ethptr->rxirq);
    printf("  Rx Octets      %8d", nicptr->rxGoodOctets);
    printf("  Rx Packets     %8d\n", nicptr->rxGoodPackets);
    printf("  Rx < 65 octets %8d", nicptr->rx_64);
    printf("  Rx < 128       %8d", nicptr->rx_65_127);
    printf("  Rx < 256       %8d\n", nicptr->rx_128_255);
    printf("  Rx < 512       %8d", nicptr->rx_256_511);
    printf("  Rx < 1024      %8d", nicptr->rx_512_1023);
    printf("  Rx >= 1024     %8d\n", nicptr->rx_1024_max);
    printf("  Rx Broadcast   %8d", nicptr->rxBroadcast);
    printf("  Rx Multicast   %8d", nicptr->rxMulticast);
    printf("  Rx Jabber      %8d\n", nicptr->rxJabber);
    printf("  Rx Oversize    %8d", nicptr->rxOversize);
    printf("  Rx Fragment    %8d", nicptr->rxFragment);
    printf("  Rx Missed      %8d\n", nicptr->rxMissed);
    printf("  Rx CRC Align   %8d", nicptr->rxCrcAlign);
    printf("  Rx Undersize   %8d", nicptr->rxUndersize);
    printf("  Rx CRC Error   %8d\n", nicptr->rxCrc);
    printf("  Rx Alignment   %8d", nicptr->rxAlign);
    printf("  Rx Symbol Err  %8d", nicptr->rxSymbol);
    printf("  Rx Pause       %8d\n", nicptr->rxPause);
    printf("  Rx Non-Pause   %8d", nicptr->rxNonPause);

    tmp = nicptr->dmaRxLast / sizeof(struct dmaDescriptor);
    printf("  Rx DMA Last    %8d", tmp);

    tmp = (nicptr->dmaRxStatus & DMARX_STAT_CDMASK)
        / sizeof(struct dmaDescriptor);
    printf("  Rx DMA Stat    %8d\n", tmp);

    printf("\n");
}

void etherThroughput(ushort minor)
{
    int tx, rx;

    struct ether *ethptr = NULL;        /* pointer to ether entry            */
    struct bcm4713 *nicptr = NULL;      /* pointer to ether device registers */

    /* Initialize pointers */
    ethptr = &ethertab[minor];
    nicptr = ethptr->csr;

    if (ethptr->dev != ethptr->phy)
    {
        return;
    }

    while (TRUE)
    {
        tx = nicptr->txGoodOctets;
        rx = nicptr->rxGoodOctets;
        sleep(1000);            /* sleep for 1 second */
        printf("Tx: %4d bytes/sec, ", nicptr->txGoodOctets - tx);
        printf("Rx: %4d bytes/sec\n", nicptr->rxGoodOctets - rx);
    }
}

static void vethStat(ushort minor)
{
    struct ether *ethptr = NULL;        /* pointer to ether entry    */
    ulong tmp;

    /* Initialize pointers */
    ethptr = &ethertab[minor];

    tmp = ethptr->state;
    printf("\n  Device ");
    if (ETH_STATE_FREE == tmp)
    {
        printf("FREE");
    }
    if (ETH_STATE_UP == tmp)
    {
        printf("UP  ");
    }
    if (ETH_STATE_DOWN == tmp)
    {
        printf("DOWN");
    }

    printf("  RxErrors %d\n\n", ethptr->rxErrors);
}
