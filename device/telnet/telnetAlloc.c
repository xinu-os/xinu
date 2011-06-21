/**
 * @file telnetAlloc.c
 * @provides telnetAlloc.
 *
 * $Id: telnetAlloc.c 2112 2009-11-02 22:15:54Z kthurow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <conf.h>
#include <stddef.h>

#include <interrupt.h>
#include <telnet.h>

/**
 * Allocate an available telnet device.
 * @return device number for a telnet device, SYSERR if none are free
 */
int telnetAlloc(void)
{
    irqmask im;
    int i;

    im = disable();
    for (i = 0; i < NTELNET; ++i)
    {
        if (TELNET_STATE_FREE == telnettab[i].state)
        {
            telnettab[i].state = TELNET_STATE_ALLOC;
            restore(im);
            return i + TELNET0;
        }
    }
    restore(im);

    return SYSERR;
}
