/**
 * @file     ethloopInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <ethloop.h>

struct ethloop elooptab[NETHLOOP];

/**
 * @ingroup ethloop
 *
 * Initialize ethloop device
 * @param devptr 
 * @return OK on open
 */
devcall ethloopInit(device *devptr)
{
    struct ethloop *elpptr;

    elpptr = &elooptab[devptr->minor];
    bzero(elpptr, sizeof(struct ethloop));
    elpptr->state = ELOOP_STATE_FREE;

    return OK;
}
