/**
 * @file teeAlloc.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>

/**
 * Find an available tee device.
 *
 * @return
 *      Device number for a tee device, or ::SYSERR if none are free.
 */
int teeAlloc(void)
{
    int dev = SYSERR;
    int i;

    for (i = 0; i < NTEE; i++)
    {
        if (!teetab[i].opened)
        {
            dev = TEE0 + i;
            break;
        }
    }
    return dev;
}
