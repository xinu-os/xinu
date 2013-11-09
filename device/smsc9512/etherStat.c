/**
 * @file     etherStat.c
 */
/* Embedded Xinu, Copyright (C) 2008, 2013.  All rights reserved. */

#include <ether.h>
#include <stdio.h>

void etherStat(ushort minor)
{
    const struct ether *ethptr = &ethertab[minor];

    printf("eth%u:\n", minor);
    printf("  MAC Address           %02X:%02X:%02X:%02X:%02X:%02X\n",
           ethptr->devAddress[0], ethptr->devAddress[1],
           ethptr->devAddress[2], ethptr->devAddress[3],
           ethptr->devAddress[4], ethptr->devAddress[5]);

    printf("  MTU                   %u\n", ethptr->mtu);

    printf("  Device state");
    switch (ethptr->state)
    {
        case ETH_STATE_FREE:
            printf("          FREE\n");
            break;
        case ETH_STATE_UP:
            printf("          UP\n");
            break;
        case ETH_STATE_DOWN:
            printf("          DOWN\n");
            break;
    }

    printf("  Rx packets in queue   %u\n",   ethptr->icount);
    printf("  Rx errors             %lu\n",  ethptr->errors);
    printf("  Rx overruns           %u\n",   ethptr->ovrrun);
    printf("  Rx USB transfers done %lu\n",  ethptr->rxirq);
    printf("  Tx USB transfers done %lu\n",  ethptr->txirq);
}

void etherThroughput(ushort minor)
{
    printf("Throughput monitoring not implemented for "
           "SMSC LAN9512 USB Ethernet Adapter\n");
}
