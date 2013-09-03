/**
 * @file read.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Read data from a device.
 *
 * @param descrp
 *      Index of the device to read from.
 * @param buffer
 *      Buffer into which to read the data.
 * @param count
 *      Maximum number of bytes to read.
 *
 * @return
 *      On success, returns the number of bytes read, which may be less than @p
 *      count in the event of a read error or end-of-file condition.
 *      Alternatively, ::SYSERR is returned if the device index is not valid or
 *      if a read error occurred before any data at all was successfully read.
 */
devcall read(int descrp, void *buffer, uint count)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->read) (devptr, buffer, count));
}
