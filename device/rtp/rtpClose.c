/**
 * @file        rtpClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <stdlib.h>
#include <rtp.h>
#include <interrupt.h>

/**
 * Close and clear a RTP device
 * @param devptr RTP device table entry
 * @return OK is RTP is closed properly, otherwise SYSERR
 */
devcall rtpClose(device *devptr)
{
    struct rtp *rtpptr;
    irqmask im;

    /* Setup and error check pointers to structures */
    rtpptr = &rtptab[devptr->minor];

    im = disable();
    /* Free the in buffer pool */
    bfpfree(rtpptr->inPool);

    /* Free the in semaphore */
    semfree(rtpptr->isem);

    /* Clear the RTP structure and the device IO block */
    bzero(rtpptr, sizeof(struct rtp));

    /* Set device state to free */
    rtpptr->state = RTP_FREE;

    restore(im);
    return OK;
}
