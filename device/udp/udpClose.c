/**
 * @file        udpClose.c
 * @provides    udpClose
 *
 * $Id
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <stdlib.h>
#include <udp.h>
#include <interrupt.h>

/**
 * Close and clear a UDP device
 * @param devptr UDP device table entry
 * @return OK is UDP is closed properly, otherwise SYSERR
 */
devcall udpClose(device *devptr)
{
    struct udp *udpptr;
    irqmask im;

    /* Setup and error check pointers to structures */
    udpptr = &udptab[devptr->minor];

    im = disable();
    /* Free the in buffer pool */
    bfpfree(udpptr->inPool);

    /* Free the in semaphore */
    semfree(udpptr->isem);

    /* Clear the UDP structure and the device IO block */
    bzero(udpptr, sizeof(struct udp));

    /* Set device state to free */
    udpptr->state = UDP_FREE;

    restore(im);
    return OK;
}
