/**
 * @file control.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Execute an I/O Control request on a device.
 *
 * @param descrp
 *      Index of the device.
 * @param func
 *      Device-specific specific control "function".
 * @param arg1
 *      Additional argument for the device-specific control function.
 * @param arg2
 *      Additional argument for the device-specific control function.
 *
 * @return
 *      Returns ::SYSERR if the device index does not correspond to an
 *      appropriate device or if the control function is not recognized;
 *      otherwise returns a request-specific value that is typically ::SYSERR on
 *      failure, but may be either ::OK or request-specific data on success.
 */
devcall control(int descrp, int func, long arg1, long arg2)
{
    device *devptr;

    if (isbaddev(descrp))
    {
        return SYSERR;
    }
    devptr = (device *)&devtab[descrp];
    return ((*devptr->control) (devptr, func, arg1, arg2));
}
