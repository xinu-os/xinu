/**
 * @file control.c
 * @provides control.
 *
 * $Id: control.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * control a device (e.g., set the mode)
 * @param descrp definition of device to control
 * @param func specific control function for device
 * @param arg1 specific argument for func
 * @param arg2 specific argument for func
 * @return function to control device on success, SYSERR on failure
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
