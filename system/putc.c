/**
 * @file putc.c
 * @provides putc.
 *
 * $Id: putc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * write a single character to a device
 * @param descrp definition of device on which to write
 * @param ch character to write
 * @return function to write character on success, SYSERR on failure
 */
devcall putc(int descrp, char ch)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->putc) (devptr, ch));
}
