/**
 * @file        udpClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <bufpool.h>
#include <stdlib.h>
#include <udp.h>
#include <interrupt.h>

/**
 * @ingroup udpexternal
 *
 * Closes a UDP device.
 *
 * Caveat:  There must not be any threads executing udpWrite() on the UDP device
 * when it is closed.  On the other hand, it is safe to close the UDP device
 * while threads are executing udpRead(), and they will return ::SYSERR.
 * However, you still must not re-open the UDP device before all udpRead()
 * threads have returned.
 *
 * @param devptr
 *      Device entry for the UDP device.
 * @return
 *      ::OK if the UDP device was closed successfully; ::SYSERR otherwise.
 *      Currently this function can only fail if the UDP device was not actually
 *      open.
 */
devcall udpClose(device *devptr)
{
    struct udp *udpptr;
    irqmask im;

    /* Get pointer to the UDP structure  */
    udpptr = &udptab[devptr->minor];

    im = disable();

    /* Make sure UDP device is actually open  */
    if (UDP_OPEN != udpptr->state)
    {
        restore(im);
        return SYSERR;
    }

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
