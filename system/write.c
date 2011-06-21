/**
 * @file write.c
 * @provides write.
 *
 * $Id: write.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * write 1 or more bytes to a device
 * @param descrp definition of device on which to write
 * @param *buffer pointer to data to be written
 * @param count length of data to be written
 * @return function to write data on success, SYSERR on failure
 */
devcall write(int descrp, void *buffer, uint count)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->write) (devptr, buffer, count));
}
