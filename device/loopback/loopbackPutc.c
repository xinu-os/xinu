/**
 * @file     loopbackPutc.c
 * @provides loopbackPutc.
 *
 * $Id: loopbackPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <semaphore.h>
#include <interrupt.h>

/**
 * Put a character onto the loopback buffer
 * @param devptr Loopback device
 * @param ch character to output
 * @return OK if character was written successfully
 */
devcall loopbackPutc(device *devptr, char ch)
{
    struct loopback *lbkptr;
    irqmask im;
    int i;

    lbkptr = &looptab[devptr->minor];

    im = disable();

    /* Ensure room in buffer */
    if (LOOP_BUFFER <= semcount(lbkptr->sem))
    {
        restore(im);
        return SYSERR;
    }

    i = (lbkptr->index + semcount(lbkptr->sem)) % LOOP_BUFFER;
    lbkptr->buffer[i] = ch;

    /* signal that more data is on the buffer */
    signal(lbkptr->sem);

    restore(im);

    return OK;
}
