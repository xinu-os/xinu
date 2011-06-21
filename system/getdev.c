/**
 * @file     getdev.c
 * @provides getdev.
 *
 * $Id: getdev.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <string.h>

/**
 * Returns the numerical identifier for the given device.
 * @param *dev device name
 * @return device number
 */
syscall getdev(char *dev)
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
