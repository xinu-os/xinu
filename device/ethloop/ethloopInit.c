/**
 * @file     ethloopInit.c
 * @provides ethloopInit
 *
 * $Id: ethloopInit.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <ethloop.h>

struct ethloop elooptab[NETHLOOP];

/**
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
