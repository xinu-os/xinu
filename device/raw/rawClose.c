/**
 * @file rawClose.c
 * @provides rawClose
 *
 * $Id: rawClose.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <raw.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * Close a raw socket device.
 * @param devptr RAW device table entry
 * @return OK if raw socket is closed properly, otherwise SYSERR
 */
devcall rawClose(device *devptr)
{
    struct raw *rawptr;
    irqmask im;

    /* Setup and error check pointers to structures */
    rawptr = &rawtab[devptr->minor];
    im = disable();
    if (RAW_ALLOC != rawptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Free semaphore */
    if (SYSERR == semfree(rawptr->isema))
    {
        restore(im);
        return SYSERR;
    }

    /* Free all pending packet buffers */
    while (rawptr->icount > 0)
    {
        netFreebuf(rawptr->in[rawptr->istart]);
        rawptr->istart = (rawptr->istart + 1) % RAW_IBLEN;
        rawptr->icount--;
    }

    bzero(rawptr, sizeof(struct raw));  /* Clear RAW structure.         */
    restore(im);
    return OK;
}
