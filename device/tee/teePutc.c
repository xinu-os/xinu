/**
 * @file teePutc.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>

/**
 * Write a single character to a tee device, meaning that the character is
 * written to all sub-devices associated with the tee device (except those that
 * have had ::TEE_SUBDEV_FLAG_WRITE_DISABLED set).
 *
 * @param devptr
 *      Pointer to the device table entry for a tee device.
 * @param ch
 *      The character to write.
 *
 * @return
 *      On success, returns the character written as an <code>unsigned
 *      char</code> cast to an @c int.  On failure, returns ::SYSERR.  By
 *      default, failure to write the character to any subdevice causes
 *      teePutc() to fail.  If instead ::TEE_FLAG_BEST_WRITE is set on the tee
 *      device, then teePutc() succeeds if the character could be successfully
 *      written to at least one subdevice.
 */
devcall teePutc(device *devptr, char c)
{
    int retval;

    retval = teeWrite(devptr, &c, 1);
    if (retval == 1)
    {
        return (uchar)c;
    }
    else
    {
        return SYSERR;
    }
}
