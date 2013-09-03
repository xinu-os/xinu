/**
 * @file rtpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <rtp.h>
#include <udp.h>

/**
 * Allocate an available rtp device.
 * @return device number for a rtp device, SYSERR if none are free
 */
ushort rtpAlloc(void)
{
    irqmask im;
    int i;

    im = disable();
    for (i = 0; i < NRTP; i++)
    {
        if (RTP_FREE == rtptab[i].state)
        {
            rtptab[i].state = RTP_ALLOC;
            restore(im);
            return i + RTP0;
        }
    }
    restore(im);

    return SYSERR;
}
