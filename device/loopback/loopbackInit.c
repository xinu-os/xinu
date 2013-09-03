/**
 * @file     loopbackInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>

struct loopback looptab[NLOOPBACK];

/**
 * @ingroup loopback
 *
 * Initialize loopback device
 * @param devptr 
 * @return OK on open
 */
devcall loopbackInit(device *devptr)
{
    struct loopback *lbkptr = NULL;

    lbkptr = &looptab[devptr->minor];
    lbkptr->state = LOOP_STATE_FREE;
    lbkptr->index = 0;

    return OK;
}
