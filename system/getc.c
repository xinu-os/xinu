/**
 * @file getc.c
 * @provides getc.
 *
 * $Id: getc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * get one character from a device
 * @param descrp definition of device from which to acquire character
 * @return function to get character on success, SYSERR on failure
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
