/**
 * @file teeGetc.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>

/**
 * Read a character from a tee device -- that is, from any one of the subdevices
 * associated with it (except those that have had
 * ::TEE_SUBDEV_FLAG_READ_DISABLED set).
 *
 * @param devptr
 *      Device table entry for the tee device.
 *
 * @return
 *      On success, returns the character read as an <code>unsigned char</code>
 *      cast to an <code>int</code>.  On error, returns ::SYSERR.
 */
devcall teeGetc(device *devptr)
{
    uchar c;
    int retval;

    retval = teeRead(devptr, &c, 1);
    if (retval == 1)
    {
        return c;
    }
    else
    {
        return SYSERR;
    }
}
