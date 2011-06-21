/**
 * @file read.c
 * @provides read.
 *
 * $Id: read.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * read one or more bytes from a device
 * @param descrp definition of device from which to read
 * @param *buffer pointer into read-to location
 * @param count length of buffer
 * @return function to read from device on success, SYSERR on failure
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
