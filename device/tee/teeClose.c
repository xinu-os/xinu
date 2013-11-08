/**
 * @file teeClose.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>
#include <interrupt.h>
#include <thread.h>

/**
 * Close a tee device.
 *
 * @param devptr
 *      Device table entry for the tee device.
 *
 * @return
 *      ::OK on success; ::SYSERR if already closed.
 */
devcall teeClose(device *devptr)
{
    irqmask im;
    struct tee *tee;
    uint i;

    im = disable();
    tee = &teetab[devptr->minor];

    if (!tee->opened)
    {
        /* Can't close device that's not open.  */
        restore(im);
        return SYSERR;
    }

    if (tee->readers_started)
    {
        /* Stop reader threads.  */
        for (i = 0; i < tee->num_subdevs; i++)
        {
            kill(tee->reader_tids[i]);
        }
        semfree(tee->isema);
    }
    tee->opened = FALSE;
    restore(im);
    return OK;

}
