/**
 * @file close.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Close a device.
 *
 * @param descrp
 *      Index of the device to close.
 *
 * @return
 *      ::OK if device was successfully closed; ::SYSERR otherwise.
 *
 * Most device drivers will return ::SYSERR if the device is not open but
 * otherwise will always be able to successfully close the device and return
 * ::OK.
 *
 * Caveat:  You must not close the device while any threads are using it (e.g.
 * for read() or write()), unless the corresponding device driver documents this
 * as allowed.
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
