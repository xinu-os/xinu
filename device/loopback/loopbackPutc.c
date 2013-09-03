/**
 * @file     loopbackPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>
#include <semaphore.h>
#include <interrupt.h>

/**
 * @ingroup loopback
 *
 * Put a character onto the loopback buffer
 *
 * @param devptr
 *      Loopback device
 * @param ch
 *      character to output
 *
 * @return
 *      @p ch as an <code>unsigned char</code> cast to an @c int on success; @c
 *      SYSERR if there is no room in the buffer.
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

    return (uchar)ch;
}
