/**
 * @file ttyOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <tty.h>
#include <stdarg.h>
#include <interrupt.h>

/**
 * @ingroup tty
 *
 * Associate a TTY with a hardware device.
 * @param devptr TTY device table entry
 * @param ap 2nd argument is the device number for the hardware device
 * @return OK if TTY is opened properly, otherwise SYSERR
 */
devcall ttyOpen(device *devptr, va_list ap)
{
    struct tty *ttyptr = NULL;
    int dvnum = 0;
    irqmask im;

    /* Second arg should be device index for physical hardware */
    dvnum = va_arg(ap, int);
    if (isbaddev(dvnum))
    {
        return SYSERR;
    }

    /* Setup pointer to tty */
    ttyptr = &ttytab[devptr->minor];

    im = disable();
    /* Check if TTY is already open */
    if (NULL != ttyptr->phw)
    {
        restore(im);
        return SYSERR;
    }

    /* Initialize input buffer */
    ttyptr->istart = 0;
    ttyptr->icount = 0;
    ttyptr->idelim = FALSE;

    /* Initialize input and output flags */
    ttyptr->iflags = TTY_ICRNL;
    ttyptr->ieof = FALSE;
    ttyptr->oflags = TTY_ONLCR;

    ttyptr->phw = (device *)&devtab[dvnum];

    restore(im);
    return OK;
}
