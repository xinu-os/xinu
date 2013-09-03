/**
 * @file     loopbackGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <semaphore.h>
#include <loopback.h>
#include <stdio.h>
#include <interrupt.h>

/**
 * @ingroup loopback
 *
 * Get a character from the loopback buffer, possibly blocking.
 *
 * @param devptr
 *      Pointer to the loopback device.
 *
 * @return
 *      The resulting @p ch as an <code>unsigned char</code> cast to an @c int
 *      on success; @c EOF if there is no data available and the device is in
 *      nonblocking mode.
 */
devcall loopbackGetc(device *devptr)
{
    irqmask im;
    struct loopback *lbkptr;
    uchar ch;

    lbkptr = &looptab[devptr->minor];

    im = disable();

    /* wait until the buffer has data */
    if (LOOP_NONBLOCK == (lbkptr->flags & LOOP_NONBLOCK)) {
        if (semcount(lbkptr->sem) <= 0) {
            restore(im);
            return EOF;
        }
    }

    wait(lbkptr->sem);

    /* Get and return the next character.  */
    ch = lbkptr->buffer[lbkptr->index];
    lbkptr->index = (lbkptr->index + 1) % LOOP_BUFFER;
    restore(im);
    return ch;
}
