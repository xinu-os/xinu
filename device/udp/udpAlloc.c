/**
 * @file udpAlloc.c
 * @provides udpAlloc.
 *
 * $Id: udpAlloc.c 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <udp.h>

/**
 * Allocate an available udp device.
 * @return device number for a udp device, SYSERR if none are free
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
