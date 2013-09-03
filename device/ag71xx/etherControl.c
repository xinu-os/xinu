/**
 * @file etherControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "ag71xx.h"
#include <ether.h>
#include <ethernet.h>
#include <network.h>

/* Implementation of etherControl() for the ag71xx; see the documentation for
 * this function in ether.h.  */
devcall etherControl(device *devptr, int func, long arg1, long arg2)
{
    struct ether *ethptr;
    struct ag71xx *nicptr;
    struct netaddr *addr;
    uchar *macptr;
    ulong temp = 0;

    ethptr = &ethertab[devptr->minor];
    if (NULL == ethptr->csr)
    {
        return SYSERR;
    }
    nicptr = ethptr->csr;

    switch (func)
    {
/* /\* Clear stats counters on ethernet device. *\/ */
/*     case ETH_CTRL_CLEAR_STATS: */
/*         nicptr->mibControl = MIB_CTRL_CLR_ON_READ; */

/*         preg = &nicptr->txGoodOctets; */
/*         while (preg <= &nicptr->txPause) */
/*         { */
/*             temp = *preg++; */
/*         } */

/*         preg = &nicptr->rxGoodOctets; */
/*         while (preg <= &nicptr->rxNonPause) */
/*         { */
/*             temp = *preg++; */
/*         } */

/*         temp = nicptr->mibControl & ~MIB_CTRL_CLR_ON_READ; */
/*         nicptr->mibControl = temp; */
/*         break; */

/* Program MAC address into card. */
    case ETH_CTRL_SET_MAC:
        macptr = (uchar *)arg1;

        temp = ((uint)macptr[0]) << 24;
        temp |= ((uint)macptr[1]) << 16;
        temp |= ((uint)macptr[2]) << 8;
        temp |= ((uint)macptr[3]) << 0;
        nicptr->macAddr1 = temp;

        temp = 0;
        temp = ((uint)macptr[4]) << 24;
        temp |= ((uint)macptr[5]) << 16;
        nicptr->macAddr2 = temp;
        break;

/* Get MAC address from card. */
    case ETH_CTRL_GET_MAC:
        macptr = (uchar *)arg1;

        temp = nicptr->macAddr1;
        macptr[0] = temp >> 24;
        macptr[1] = temp >> 16;
        macptr[2] = temp >> 8;
        macptr[3] = temp >> 0;

        temp = nicptr->macAddr2;
        macptr[4] = temp >> 24;
        macptr[5] = temp >> 16;
        break;

/* Set receiver mode. */
    case ETH_CTRL_SET_LOOPBK:
        if (TRUE == (uint)arg1)
        {
            nicptr->macConfig1 |= MAC_CFG1_LOOPBACK;
        }
        else
        {
            nicptr->macConfig1 &= ~MAC_CFG1_LOOPBACK;
        }
        break;

/* /\* Reset Ethernet device. *\/ */
/*     case ETH_CTRL_RESET: */
/*         /\* Idle the network card if the backplane core is up *\/ */
/*         if (backplaneCoreUp(&(nicptr->bpConfig))) */
/*         { */
/*             etherControl(devptr, ETH_CTRL_DISABLE, 0, 0); */
/*         } */

/*         /\* Reset the target core. *\/ */
/*         backplaneReset(&(nicptr->bpConfig)); */

/*         etherControl(devptr, ETH_CTRL_CLEAR_STATS, 0, 0); */

/*         /\* Make PHY accessible and set frequency *\/ */
/*         nicptr->mdioControl = (MDIO_CTRL_PREAMBLE | MDIO_CTRL_FREQ); */

/*         /\* Select external PHY, this model does not have internal. *\/ */
/*         nicptr->enetControl = ENET_CTRL_EPSEL; */
/*         break; */

/* /\* Disable ethernet core *\/ */
/*     case ETH_CTRL_DISABLE: */
/*         nicptr->rcvLazy = 0;     /\* Turn off lazy reception. *\/ */
/*         nicptr->enetControl;     /\* Read control register.   *\/ */

/*         /\* Wait for controller *\/ */
/*         waitOnBit(&nicptr->enetControl, ENET_CTRL_DISABLE, 0, 100); */
/*         /\* Reset transmitter settings and transmitter queue. *\/ */
/*         nicptr->dmaTxControl = 0; */
/*         ethptr->txHead = 0; */
/*         ethptr->txTail = 0; */
/*         if (nicptr->dmaRxStatus & DMARX_STAT_EMASK) */
/*         { */
/*             /\* Wait for receiver to idle. *\/ */
/*             waitOnBit(&nicptr->dmaRxStatus, DMARX_STAT_SIDLE, 1, 100); */
/*         } */

/*         /\* Reset receiver settings and receiver queue. *\/ */
/*         nicptr->dmaRxControl = 0; */
/*         ethptr->rxHead = 0; */
/*         ethptr->rxTail = 0; */
/*         break; */

/* Get link header length. */
    case NET_GET_LINKHDRLEN:
        return ETH_HDR_LEN;

/* Get MTU. */
    case NET_GET_MTU:
        return ETH_MTU;

    case NET_GET_HWADDR:
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        etherControl(devptr, ETH_CTRL_GET_MAC, (int)addr->addr, NULL);
        break;

/* Get broadcast MAC address. */
    case NET_GET_HWBRC:
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        addr->addr[0] = 0xFF;
        addr->addr[1] = 0xFF;
        addr->addr[2] = 0xFF;
        addr->addr[3] = 0xFF;
        addr->addr[4] = 0xFF;
        addr->addr[5] = 0xFF;
        break;

    default:
        return SYSERR;
    }

    return OK;
}
