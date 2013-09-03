/**
 * @file open.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdarg.h>

/**
 * @ingroup devcalls
 *
 * Open a device so that read(), write(), putc(), and/or getc() operations can
 * be performed on it.
 *
 * @param descrp
 *      Index of the device to open.
 * @param ...
 *      Zero or more additional parameters that will be passed to the
 *      device-specific open function.
 *
 * @return
 *      On success, returns ::OK.  If the device index is bad or if another
 *      error occurs, returns ::SYSERR.  Generally, device drivers will at least
 *      return ::SYSERR in the case that the device is already open, but
 *      ::SYSERR may also be returned because of failure to allocate resources
 *      or for device-specific errors.
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
