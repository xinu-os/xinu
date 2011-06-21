/**
 * @file     loopbackGetc.c
 * @provides loopbackGetc.
 *
 * $Id: loopbackGetc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <semaphore.h>
#include <loopback.h>
#include <stdio.h>
#include <interrupt.h>

/**
 * Get a character from the loopback buffer
 * @param devptr Loopback device
 * @return character from buffer
 */
devcall loopbackGetc(device *devptr)
{
    irqmask im;
    struct loopback *lbkptr;
    int i;

    lbkptr = &looptab[devptr->minor];

    /* wait until the buffer has data */
    wait(lbkptr->sem);

    im = disable();

    i = lbkptr->index;
    lbkptr->index = (i + 1) % LOOP_BUFFER;

    restore(im);

    /* consume the data */
    return lbkptr->buffer[i];
}
