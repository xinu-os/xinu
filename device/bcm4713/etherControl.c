/**
 * @file etherControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <backplane.h>
#include <ethernet.h>
#include <network.h>

/* Implementation of etherControl() for the bcm4713; see the documentation for
 * this function in ether.h.  */
devcall etherControl(device *devptr, int func, long arg1, long arg2)
{
    struct ether *ethptr;
    struct bcm4713 *nicptr;
    volatile uint *regptr;
    uchar *macptr;
    ulong temp = 0;
    struct netaddr *addr;

    ethptr = &ethertab[devptr->minor];
    if (NULL == ethptr->csr)
    {
        return SYSERR;
    }
    nicptr = ethptr->csr;

    switch (func)
    {
/* Clear stats counters on ethernet device. */
    case ETH_CTRL_CLEAR_STATS:
        nicptr->mibControl = MIB_CTRL_CLR_ON_READ;

        regptr = &nicptr->txGoodOctets;
        while (regptr <= &nicptr->txPause)
        {
            temp = *regptr++;
        }

        regptr = &nicptr->rxGoodOctets;
        while (regptr <= &nicptr->rxNonPause)
        {
            temp = *regptr++;
        }

        temp = nicptr->mibControl & ~MIB_CTRL_CLR_ON_READ;
        nicptr->mibControl = temp;
        break;

/* Get link header length. */
    case NET_GET_LINKHDRLEN:
        return ETH_HDR_LEN;

/* Program MAC address into card. */
    case ETH_CTRL_SET_MAC:
        macptr = (uchar *)arg1;
        /* b44 cam write */
        nicptr->camControl = 0;
        temp = ((uint)macptr[2]) << 24;
        temp |= ((uint)macptr[3]) << 16;
        temp |= ((uint)macptr[4]) << 8;
        temp |= ((uint)macptr[5]) << 0;
        nicptr->camDataLo = temp;

        temp = (CAM_DATA_HI_VALID
                | (((uint)macptr[0]) << 8) | (((uint)macptr[1]) << 0));
        nicptr->camDataHi = temp;
        nicptr->camControl =
            (devptr->minor << CAM_CTRL_INDEX_SHIFT) | CAM_CTRL_WRITE;

        waitOnBit(&nicptr->camControl, CAM_CTRL_BUSY, 0, 100);

        /* Re-enable CAM control */
        nicptr->camControl |= CAM_CTRL_ENABLE;
        break;

/* Get MAC address from card. */
    case ETH_CTRL_GET_MAC:
        macptr = (uchar *)arg1;
        nicptr->camControl =
            (devptr->minor << CAM_CTRL_INDEX_SHIFT) | CAM_CTRL_READ;
        waitOnBit(&nicptr->camControl, CAM_CTRL_BUSY, 0, 100);

        temp = nicptr->camDataLo;
        macptr[2] = temp >> 24;
        macptr[3] = temp >> 16;
        macptr[4] = temp >> 8;
        macptr[5] = temp >> 0;

        temp = nicptr->camDataHi;
        macptr[0] = temp >> 8;
        macptr[1] = temp >> 0;

        /* Re-enable CAM control */
        nicptr->camControl = CAM_CTRL_ENABLE;
        break;

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

/* Set receiver mode. */
    case ETH_CTRL_SET_LOOPBK:
        if (TRUE == (uint)arg1)
        {
            nicptr->rxConfig = RXCONFIG_LPBACK;
        }
        else
        {
            nicptr->rxConfig = 0;
        }
        break;

/* Reset Ethernet device. */
    case ETH_CTRL_RESET:
        /* Idle the network card if the backplane core is up */
        if (backplaneCoreUp(&(nicptr->bpConfig)))
        {
            etherControl(devptr, ETH_CTRL_DISABLE, 0, 0);
        }

        /* Reset the target core. */
        backplaneReset(&(nicptr->bpConfig));

        etherControl(devptr, ETH_CTRL_CLEAR_STATS, 0, 0);

        /* Make PHY accessible and set frequency */
        nicptr->mdioControl = (MDIO_CTRL_PREAMBLE | MDIO_CTRL_FREQ);

        /* Select external PHY, this model does not have internal. */
        nicptr->enetControl = ENET_CTRL_EPSEL;
        break;

/* Disable ethernet core */
    case ETH_CTRL_DISABLE:
        nicptr->rcvLazy = 0;    /* Turn off lazy reception. */
        nicptr->enetControl;    /* Read control register.   */

        /* Wait for controller */
        waitOnBit(&nicptr->enetControl, ENET_CTRL_DISABLE, 0, 100);
        /* Reset transmitter settings and transmitter queue. */
        nicptr->dmaTxControl = 0;
        ethptr->txHead = 0;
        ethptr->txTail = 0;
        if (nicptr->dmaRxStatus & DMARX_STAT_EMASK)
        {
            /* Wait for receiver to idle. */
            waitOnBit(&nicptr->dmaRxStatus, DMARX_STAT_SIDLE, 1, 100);
        }

        /* Reset receiver settings and receiver queue. */
        nicptr->dmaRxControl = 0;
        ethptr->rxHead = 0;
        ethptr->rxTail = 0;
        break;

/* Get MTU. */
    case NET_GET_MTU:
        return ETH_MTU;

    default:
        return SYSERR;
    }

    return OK;
}
