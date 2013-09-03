/**
 * @file httpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <conf.h>
#include <stddef.h>

#include <interrupt.h>
#include <http.h>

/**
 * Allocate an available http device.
 * @return device number for a http device, SYSERR if none are free
 */
int httpAlloc(void)
{
    irqmask ps;
    int i;

    ps = disable();
    for (i = 0; i < NHTTP; ++i)
    {
        if (HTTP_STATE_FREE == httptab[i].state)
        {
            httptab[i].state = HTTP_STATE_ALLOC;
            restore(ps);
            return i + HTTP0;
        }
    }
    restore(ps);

    return SYSERR;
}
