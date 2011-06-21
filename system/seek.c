/**
 * @file seek.c
 * @provides seek.
 *
 * $Id: seek.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * position a device (very common special case of control)
 * @param descrp definition of device on which to seek
 * @param pos requested position to seek
 * @return function to seek on device on success, SYSERR on failure
 */
devcall seek(int descrp, uint pos)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->seek) (devptr, pos));
}
