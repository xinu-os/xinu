/**
 * @file udpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <udp.h>

/**
 * @ingroup udpexternal
 *
 * Allocate an available UDP device.
 *
 * @return
 *      Device number for a UDP device, or ::SYSERR if none are free.
 */
ushort udpAlloc(void)
{
    irqmask im;
    int i;

    im = disable();
    for (i = 0; i < NUDP; i++)
    {
        if (UDP_FREE == udptab[i].state)
        {
            udptab[i].state = UDP_ALLOC;
            restore(im);
            return i + UDP0;
        }
    }
    restore(im);

    return SYSERR;
}
