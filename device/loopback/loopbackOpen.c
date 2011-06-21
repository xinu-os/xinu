/**
 * @file     loopbackOpen.c
 * @provides loopbackOpen.
 * 
 * $Id: loopbackOpen.c 2077 2009-09-24 23:58:54Z mschul $
 **/
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * Open a loopback device. 
 * @param devptr loopback device table entry
 * @return OK if loopback is opened properly, otherwise SYSERR
 */
devcall loopbackOpen(device *devptr)
{
    struct loopback *lbkptr = NULL;
    irqmask im;

    lbkptr = &looptab[devptr->minor];

    im = disable();
    /* Check if loopback is already open */
    if (LOOP_STATE_FREE != lbkptr->state)
    {
        restore(im);
        return SYSERR;
    }

    lbkptr->state = LOOP_STATE_ALLOC;

    /* Gather new semaphore */
    lbkptr->sem = semcreate(0);

    /* Zero out the buffer */
    bzero(lbkptr->buffer, LOOP_BUFFER);

    /* Zero out flags */
    lbkptr->flags = 0;

    restore(im);
    return OK;
}
