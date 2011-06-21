/**
 * @file vlanOpen.c
 * @provides vlanOpen.
 *
 * $Id: vlanOpen.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <memory.h>
#include <bufpool.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * Open a virtual ethernet device for use
 * @param  devptr ethernet device table entry
 * @return OK if opened properly, otherwise SYSERR
 */
devcall vlanOpen(device *devptr)
{
    struct ether *ethptr;

    /* Initialize structure pointers */
    ethptr = &ethertab[devptr->minor];

    /* Rx and Tx buffers associated with hardware device */
    ethptr->inPool = ETH_INVALID;
    ethptr->outPool = ETH_INVALID;
    ethptr->state = ETH_STATE_UP;

    etherControl(devptr, ETH_CTRL_SET_MAC, (ulong)ethptr->devAddress, 0);

    return OK;
}
