/**
 * @file ethloopClose.c
 * @provides ethloopClose
 *
 * $Id: ethloopClose.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <semaphore.h>
#include <interrupt.h>

/**
 * Close a ethloop device.
 * @param devptr ethloop device table entry
 * @return OK if ethloop is closed properly, otherwise SYSERR
 **/
devcall ethloopClose(device *devptr)
{
    struct ethloop *elpptr;
    irqmask im;


    /* Setup and error check pointers to structures */
    elpptr = &elooptab[devptr->minor];

    im = disable();
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* free the semaphore */
    semfree(elpptr->sem);

    /* free the buffer pool */
    bfpfree(elpptr->poolid);

    elpptr->dev = NULL;
    elpptr->state = ELOOP_STATE_FREE;
    restore(im);
    return OK;
}
