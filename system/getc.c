/**
 * @file getc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Read one character from a device.
 *
 * @param descrp
 *      Index of device from which to read the character.
 *
 * @return
 *      On success, returns the character read as an <code>unsigned char</code>
 *      cast to an @c int.  On bad device descripter, returns ::SYSERR.  On
 *      other failure, returns ::SYSERR or ::EOF depending on the specific
 *      device driver it calls.
 */
devcall getc(int descrp)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->getc) (devptr));
}
