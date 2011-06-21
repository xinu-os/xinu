/**
 * @file     ethloopOpen.c
 * @provides ethloopOpen
 * 
 * $Id: ethloopOpen.c 2077 2009-09-24 23:58:54Z mschul $
 **/
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * Open an ethloop device. 
 * @param devptr ethloop device table entry
 * @return OK if ethloop is opened properly, otherwise SYSERR
 */
devcall ethloopOpen(device *devptr)
{
    struct ethloop *elpptr = NULL;
    irqmask im;

    elpptr = &elooptab[devptr->minor];

    /* Check if ethloop is already open */
    im = disable();
    if (ELOOP_STATE_FREE != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }
    elpptr->state = ELOOP_STATE_ALLOC;
    /* Link ethloop record with device table entry */
    elpptr->dev = devptr;

    /* Clear flags and stats */
    elpptr->flags = 0;
    elpptr->nout = 0;

    /* Create new semaphore */
    elpptr->sem = semcreate(0);
    elpptr->hsem = semcreate(0);
    if ((SYSERR == (int)elpptr->sem) || (SYSERR == (int)elpptr->hsem))
    {
        restore(im);
        return SYSERR;
    }

    /* Zero out the buffer */
    bzero(elpptr->buffer, ELOOP_NBUF);
    bzero(elpptr->pktlen, ELOOP_NBUF);
    elpptr->index = 0;
    elpptr->hold = NULL;
    elpptr->holdlen = 0;
    elpptr->count = 0;

    /* Allocate a buffer pool */
    elpptr->poolid = bfpalloc(ELOOP_BUFSIZE, ELOOP_NBUF);
    if (SYSERR == elpptr->poolid)
    {
        restore(im);
        return SYSERR;
    }
    restore(im);
    return OK;
}
