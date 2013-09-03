/**
 * @file     loopbackOpen.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * @ingroup loopback
 *
 * Open a loopback device. 
 * @param devptr loopback device table entry
 * @return OK if loopback is opened properly, otherwise SYSERR
 */
devcall loopbackOpen(device *devptr)
{
    struct loopback *lbkptr;
    irqmask im;

    lbkptr = &looptab[devptr->minor];

    im = disable();
    /* Check if loopback is already open */
    if (LOOP_STATE_FREE != lbkptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Create new semaphore */
    lbkptr->sem = semcreate(0);

    if (SYSERR == lbkptr->sem)
    {
        restore(im);
        return SYSERR;
    }

    /* Zero out the buffer */
    bzero(lbkptr->buffer, LOOP_BUFFER);

    /* Zero out flags */
    lbkptr->flags = 0;

    lbkptr->state = LOOP_STATE_ALLOC;

    restore(im);
    return OK;
}
