/**
 * @file open.c
 * @provides open.
 *
 * $Id: open.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdarg.h>

/**
 * open a connection to a device 
 * @param descrp definition of device to open
 * @return function to open device on success, SYSERR on failure
 */
devcall open(int descrp, ...)
{
    device *devptr;
    va_list ap;
    devcall result;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    va_start(ap, descrp);
    result = ((*devptr->open) (devptr, ap));
    va_end(ap);
    return result;
}
