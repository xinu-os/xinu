/**
 * @file httpOpen.c
 * @provides httpOpen
 *
 * $Id: httpOpen.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <http.h>
#include <interrupt.h>
#include <semaphore.h>

/**
 * Associate a HTTP with a hardware device.
 * @param devptr HTTP device table entry
 * @param ap 2nd argument is the device number for the hardware device
 * @return OK if HTTP is opened properly, otherwise SYSERR
 */
devcall httpOpen(device *devptr, va_list ap)
{
    irqmask im;
    struct http *webptr = NULL;
    int dvnum = 0;

    /* Second arg should be device index for physical hardware */
    dvnum = va_arg(ap, int);
    if (isbaddev(dvnum))
    {
        return SYSERR;
    }

    /* Setup pointer to http */
    webptr = &httptab[devptr->minor];
    im = disable();

    /* Check if HTTP is already open */
    if (HTTP_STATE_FREE != webptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Zero out the memory space of the structure */
    bzero(webptr, sizeof(struct http));

    /* Reset state to allocated */
    webptr->state = HTTP_STATE_ALLOC;
    /* Initialize awaiting request flag */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_AWAITINGRQST, NULL);

    restore(im);

    /* Initialize underlying hardward device pointer */
    webptr->phw = (device *)&devtab[dvnum];

    /* Initialize buffer counters */
    webptr->rstart = 0;
    webptr->rcount = 0;
    webptr->wstart = 0;
    webptr->wcount = 0;

    /* Acquire semaphore to wait on for closing connections */
    webptr->closeall = semcreate(0);

    return OK;
}
