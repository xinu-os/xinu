/**
 * @file     getdev.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <string.h>

/**
 * @ingroup devcalls
 *
 * Translates a device name into a device number.
 *
 * @param dev
 *      Name of the device.
 *
 * @return
 *      The number of the device, or ::SYSERR if the device was not found.
 */
syscall getdev(const char *dev)
{
    int devnum;

    for (devnum = 0; devnum < NDEVS; devnum++)
    {
        if (0 == strncmp(dev, devtab[devnum].name, DEVMAXNAME))
        {
            return devnum;
        }
    }

    return SYSERR;
}
