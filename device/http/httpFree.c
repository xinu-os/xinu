/**
 * @file httpFree.c
 * @provides httpFree
 *
 * $Id: httpFree.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>
#include <interrupt.h>
#include <semaphore.h>

/**
 * Free an allocated HTTP device.
 * @param devptr pointer to HTTP device
 * @return OK if HTTP is freed properly, otherwise SYSERR
 */
int httpFree(device *devptr)
{
    irqmask im;

    im = disable();
    httptab[devptr->minor].state = HTTP_STATE_FREE;
    restore(im);

    /* Signal the counter for HTTP threads */
    signal(maxhttp);

    return OK;
}
