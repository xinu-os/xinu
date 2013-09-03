/**
 * @file     ethloopOpen.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <interrupt.h>
#include <semaphore.h>
#include <stdlib.h>

/**
 * @ingroup ethloop
 *
 * Open an ethloop device. 
 * @param devptr ethloop device table entry
 * @return OK if ethloop is opened properly, otherwise SYSERR
 */
devcall ethloopOpen(device *devptr)
{
    struct ethloop *elpptr;
    irqmask im;
    int retval = SYSERR;

    elpptr = &elooptab[devptr->minor];
    im = disable();

    /* Make sure the ethloop is actually closed */
    if (ELOOP_STATE_FREE != elpptr->state)
    {
        goto out_restore;
    }

    /* Clear flags and stats */
    elpptr->flags = 0;
    elpptr->nout = 0;

    /* Create semaphores */
    elpptr->sem = semcreate(0);
    if (SYSERR == (int)elpptr->sem)
    {
        goto out_restore;
    }

    elpptr->hsem = semcreate(0);
    if (SYSERR == (int)elpptr->hsem)
    {
        goto out_free_sem;
    }

    /* Initialize buffers */
    bzero(elpptr->buffer, sizeof(elpptr->buffer));
    bzero(elpptr->pktlen, sizeof(elpptr->pktlen));
    elpptr->index = 0;
    elpptr->hold = NULL;
    elpptr->holdlen = 0;
    elpptr->count = 0;

    /* Allocate a buffer pool  */
    elpptr->poolid = bfpalloc(ELOOP_BUFSIZE, ELOOP_NBUF);
    if (SYSERR == elpptr->poolid)
    {
        goto out_free_hsem;
    }

    /* Link ethloop record with device table entry and mark ethloop as open */
    elpptr->state = ELOOP_STATE_ALLOC;
    elpptr->dev = devptr;

    /* Success */
    retval = OK;
    goto out_restore;

out_free_hsem:
    semfree(elpptr->hsem);
out_free_sem:
    semfree(elpptr->sem);
out_restore:
    restore(im);
    return retval;
}
