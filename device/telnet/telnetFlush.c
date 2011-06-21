/**
 * @file telnetFlush.c
 * @provides telnetFlush.
 *
 * $Id: telnetFlush.c 2115 2009-11-03 02:31:19Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <interrupt.h>
#include <telnet.h>

/**
 * Flushes the telnet server's output buffer
 * @param devptr TELNET device table entry
 * @return OK if flush is successful, SYSERR on failure
 */
devcall telnetFlush(device *devptr)
{
    struct telnet *tntptr;
    device *phw;
    irqmask im;

    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
    im = disable();

    if (NULL == phw)
    {
        return SYSERR;
    }

    if (TELNET_STATE_OPEN != tntptr->state)
    {
        return SYSERR;
    }

    if (tntptr->ostart > 0)
    {
        if (SYSERR ==
            (*phw->write) (phw, (void *)(tntptr->out), tntptr->ostart))
        {
            restore(im);
            return SYSERR;
        }

        tntptr->ostart = 0;
    }

    restore(im);

    return OK;
}
