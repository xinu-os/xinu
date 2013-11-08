/**
 * @file teeOpen.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>
#include <interrupt.h>
#include <string.h>

/** Table of tee device control blocks  */
struct tee teetab[NTEE];

/**
 * Associate a tee device with zero or more sub-devices.
 *
 * @param devptr
 *      Device table entry for the tee device.
 * @param ap
 *      Variable arguments.  The first shall be a non-negative integer giving
 *      the number of remaining arguments, while any remaining arguments shall
 *      be the device IDs of devices to associate with the tee device.
 *
 * @return
 *      ::OK on success; ::SYSERR if invalid parameters were passed or if the
 *      tee device was already open.
 */
devcall teeOpen(device *devptr, va_list ap)
{
    struct tee *tee;
    uint num_subdevs;
    uint i;
    irqmask im;

    im = disable();
    tee = &teetab[devptr->minor];

    if (tee->opened)
    {
        /* Can't open already-open device.  */
        restore(im);
        return SYSERR;
    }

    /* Start by zeroing the tee structure.  */
    memset(tee, 0, sizeof(struct tee));

    /* Get number of subdevices requested.  */
    num_subdevs = va_arg(ap, uint);
    if (num_subdevs > TEE_MAX_SUBDEVS)
    {
        /* Too many subdevices requested  */
        restore(im);
        return SYSERR;
    }

    /* Save and verify the subdevices.  */
    for (i = 0; i < num_subdevs; i++)
    {
        tee->subdevs[i] = va_arg(ap, int);
        if (isbaddev(tee->subdevs[i]))
        {
            restore(im);
            return SYSERR;
        }
    }
    /* Finish initializations and return success.  */
    tee->num_subdevs = num_subdevs;
    tee->opened = TRUE;
    restore(im);
    return OK;
}
