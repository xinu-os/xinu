/**
 * @file ethloopClose.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <interrupt.h>
#include <semaphore.h>

/**
 * @ingroup ethloop
 *
 * Close a ethloop device.
 * @param devptr ethloop device table entry
 * @return OK if ethloop is closed properly, otherwise SYSERR
 */
devcall ethloopClose(device *devptr)
{
    struct ethloop *elpptr;
    irqmask im;

    elpptr = &elooptab[devptr->minor];
    im = disable();

    /* Make sure the ethloop is actually open */
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* free the semaphores */
    semfree(elpptr->sem);
    semfree(elpptr->hsem);

    /* free the buffer pool */
    bfpfree(elpptr->poolid);

    /* mark as not open */
    elpptr->dev = NULL;
    elpptr->state = ELOOP_STATE_FREE;
    restore(im);
    return OK;
}
