/**
 * @file etherClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include "ag71xx.h"
#include <ether.h>
#include <memory.h>
#include <bufpool.h>
#include <stdlib.h>
#include <interrupt.h>
#include <thread.h>

/* Implementation of etherClose() for the ag71xx; see the documentation for this
 * function in ether.h.  */
devcall etherClose(device *devptr)
{
    irqmask im;
    struct ether *ethptr = NULL;
    struct ag71xx *nicptr = NULL;

    ethptr = &ethertab[devptr->minor];
    nicptr = ethptr->csr;

    /* flag interface as down */
    ethptr->state = ETH_STATE_DOWN;

    im = disable();

    /* disable ether interrupt source */
    nicptr->interruptMask = ethptr->interruptMask = 0x0;
    nicptr->interruptStatus = 0x0;

    restore(im);

    etherControl(devptr, ETH_CTRL_RESET, 0, 0);

    bfpfree(ethptr->inPool);
    bfpfree(ethptr->outPool);

    return OK;
}
