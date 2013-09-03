/**
 * @file vlanClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <bufpool.h>
#include <stdlib.h>

/**
 * @ingroup etherspecific
 *
 * Close a virtual ethernet device.
 * @param devptr ether device table entry
 * @return OK if device is closed properly
 */
devcall vlanClose(device *devptr)
{
    struct ether *ethptr = NULL;

    ethptr = &ethertab[devptr->minor];

    /* flag interface as down */
    ethptr->state = ETH_STATE_DOWN;

    return OK;
}
