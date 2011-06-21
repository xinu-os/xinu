/**
 * @file etherInit.c
 * @provides etherInit.
 *
 * $Id: etherInit.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <mips.h>
#include <memory.h>
#include <backplane.h>
#include "bcm4713.h"
#include <ether.h>
#include "bcmswitch.h"
#include <nvram.h>
#include <safemem.h>

/* Global table of ethernet devices */
struct ether ethertab[NETHER];

/**
 * Initialize ethernet device structures.
 * @param devptr ETH device table entry
 * @return OK if device is intialized successfully, otherwise SYSERR
 */
devcall etherInit(device *devptr)
{
    struct ether *ethptr;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];
    bzero(ethptr, sizeof(struct ether));
    ethptr->dev = devptr;
    ethptr->csr = devptr->csr;
    ethptr->phy = devptr;

    ethptr->state = ETH_STATE_DOWN;
    ethptr->rxRingSize = ETH_RX_RING_ENTRIES;
    ethptr->txRingSize = ETH_TX_RING_ENTRIES;
    ethptr->mtu = ETH_MTU;
    ethptr->interruptMask = IMASK_DEF;

    ethptr->errors = 0;
    ethptr->isema = semcreate(0);
    ethptr->istart = 0;
    ethptr->icount = 0;
    ethptr->ovrrun = 0;
    ethptr->rxOffset = sizeof(struct rxHeader);

    /* Lookup canonical MAC in NVRAM, and store in ether struct */
    colon2mac(nvramGet("et0macaddr"), ethptr->devAddress);

    /* NOTE: because device initialization runs early in the system, */
    /*  we are assured that this stkget() call will return          */
    /*  page-aligned (and cache-aligned) boundaries.                */
    ethptr->rxBufs = stkget(PAGE_SIZE);
    ethptr->txBufs = stkget(PAGE_SIZE);
    ethptr->rxRing = stkget(PAGE_SIZE);
    ethptr->txRing = stkget(PAGE_SIZE);

    if ((SYSERR == (int)ethptr->rxBufs)
        || (SYSERR == (int)ethptr->txBufs)
        || (SYSERR == (int)ethptr->rxRing)
        || (SYSERR == (int)ethptr->txRing))
    {
#ifdef DETAIL
        kprintf("eth%d ring buffer allocation error.\r\n", devptr->minor);
#endif                          /* DETAIL */
        return SYSERR;
    }

    /* bump buffers/rings to KSEG1 */
    ethptr->rxBufs =
        (struct ethPktBuffer
         **)(((ulong)ethptr->rxBufs - PAGE_SIZE +
              sizeof(int)) | KSEG1_BASE);
    ethptr->txBufs =
        (struct ethPktBuffer
         **)(((ulong)ethptr->txBufs - PAGE_SIZE +
              sizeof(int)) | KSEG1_BASE);
    ethptr->rxRing =
        (struct dmaDescriptor
         *)(((ulong)ethptr->rxRing - PAGE_SIZE +
             sizeof(int)) | KSEG1_BASE);
    ethptr->txRing =
        (struct dmaDescriptor
         *)(((ulong)ethptr->txRing - PAGE_SIZE +
             sizeof(int)) | KSEG1_BASE);

    /* Make sure nothing erroneous is in buffers/rings */
    bzero(ethptr->rxBufs, PAGE_SIZE);
    bzero(ethptr->txBufs, PAGE_SIZE);
    bzero(ethptr->rxRing, PAGE_SIZE);
    bzero(ethptr->txRing, PAGE_SIZE);

    /* Reset ethernet device to known state. */
    etherControl(devptr, ETH_CTRL_RESET, 0, 0);

    /* Initialize the network switch */
    switchInit((struct bcm4713 *)ethptr->csr);

    interruptVector[devptr->irq] = devptr->intr;
    enable_irq(devptr->irq);

    return OK;
}
