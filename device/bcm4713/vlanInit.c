/**
 * @file vlanInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <device.h>
#include "bcm4713.h"
#include <ether.h>
#include <vlan.h>
#include <memory.h>
#include <backplane.h>
#include <nvram.h>

/**
 * @ingroup etherspecific
 *
 * Initialize virtual ethernet device structures.
 * @param devptr TTY device table entry
 * @return OK if device is intialized successfully
 */
devcall vlanInit(device *devptr)
{
    struct ether *ethptr;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];
    bzero(ethptr, sizeof(struct ether));
    ethptr->dev = devptr;
    ethptr->csr = (struct bcm4713 *)devptr->csr;
    /* Physical ethernet device to which the VLAN attaches */
    ethptr->phy = (device *)&devtab[ETH0];

    ethptr->errors = 0;
    ethptr->state = ETH_STATE_DOWN;
    ethptr->rxRingSize = ETH_RX_RING_ENTRIES;
    ethptr->txRingSize = ETH_TX_RING_ENTRIES;
    ethptr->mtu = ETH_MTU;

    ethptr->isema = semcreate(0);
    ethptr->istart = 0;
    ethptr->icount = 0;
    ethptr->ovrrun = 0;
    ethptr->rxOffset = sizeof(struct rxHeader);

    /* Lookup MAC in NVRAM, and store it in ether dev struct. */
    /* Modify first octet with device minor and set local admin bit */
    colon2mac(nvramGet("et0macaddr"), ethptr->devAddress);
    ethptr->devAddress[0] |= ((uchar)devptr->minor << 2) | ETH_LOCAL_MAC;

    ethptr->interruptMask = IMASK_DEF;

    /* As there is only one DMA ring for the physical ethernet, and
     * none for virtual interfaces, certain values in the ether struct
     * are set to invalid
     */
    ethptr->rxBufs = (struct ethPktBuffer **)ETH_INVALID;
    ethptr->txBufs = (struct ethPktBuffer **)ETH_INVALID;
    ethptr->rxRing = (struct dmaDescriptor *)ETH_INVALID;
    ethptr->txRing = (struct dmaDescriptor *)ETH_INVALID;

    return OK;
}
