/**
 * @file close.c
 * @provides close.
 *
 * $Id: close.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * close a device
 * @param descrp definition of device to close
 * @return function to close device on success, SYSERR on failure
 */
devcall close(int descrp)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->close) (devptr));
}
