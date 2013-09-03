/**
 * @file ttyClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <tty.h>
#include <stdlib.h>
#include <interrupt.h>

/**
 * @ingroup tty
 *
 * Close a TTY device.
 * @param devptr TTY device table entry
 * @return OK if TTY is closed properly, otherwise SYSERR
 */
devcall ttyClose(device *devptr)
{
    struct tty *ttyptr;
    irqmask im;

    im = disable();
    /* Setup and error check pointers to structures */
    ttyptr = &ttytab[devptr->minor];
    bzero(ttyptr, sizeof(struct tty));  /* Clear TTY structure.         */
    restore(im);
    return OK;
}
