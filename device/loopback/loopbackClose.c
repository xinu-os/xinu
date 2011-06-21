/**
 * @file loopbackClose.c
 * @provides loopbackClose.
 *
 * $Id: loopbackClose.c 2077 2009-09-24 23:58:54Z mschul $
 **/
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * Close a loopback device.
 * @param devptr loopback device table entry
 * @return OK if loopback is closed properly, otherwise SYSERR
 **/
devcall loopbackClose(device *devptr)
{
    struct loopback *lbkptr;
    irqmask im;

    lbkptr = &looptab[devptr->minor];

    im = disable();
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* free the semaphore */
    semfree(lbkptr->sem);
    lbkptr->state = LOOP_STATE_FREE;

    restore(im);
    return OK;
}
