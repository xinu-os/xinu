/**
 * @file vlanClose.c
 * @provides vlanClose.
 *
 * $Id: vlanClose.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <bufpool.h>
#include <stdlib.h>

/**
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
