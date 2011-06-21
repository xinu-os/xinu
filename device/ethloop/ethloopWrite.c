/**
 * @file     ethloopWrite.c
 * @provides ethloopWrite
 *
 * $Id: ethloopWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <interrupt.h>
#include <string.h>

/**
 * Write characters to an ethloop.
 * @param devptr 
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters written, SYSERR if an error occurs
 */
devcall ethloopWrite(device *devptr, void *buf, uint len)
{
    struct ethloop *elpptr;
    irqmask im;
    int index;
    char *pkt;

    elpptr = &elooptab[devptr->minor];

    if ((len > ELOOP_BUFSIZE) || (len < ELOOP_LINKHDRSIZE))
    {
        return SYSERR;
    }

    im = disable();
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Allocate buffer space */
    pkt = (char *)bufget(elpptr->poolid);
    if (SYSERR == (int)pkt)
    {
        return SYSERR;
    }

    /* Copy supplied buffer into allocated buffer */
    memcpy(pkt, buf, len);

    /* Drop packet if drop flags(s) are set */
    if ((elpptr->flags & ELOOP_FLAG_DROPNXT)
        || (elpptr->flags & ELOOP_FLAG_DROPALL))
    {
        elpptr->flags &= ~ELOOP_FLAG_DROPNXT;
        restore(im);
        return len;
    }

    /* Hold next packet if flags is set */
    if (elpptr->flags & ELOOP_FLAG_HOLDNXT)
    {
        elpptr->flags &= ~ELOOP_FLAG_HOLDNXT;
        if (elpptr->hold != NULL)
        {
            buffree(elpptr->hold);
        }
        elpptr->hold = pkt;
        elpptr->holdlen = len;
        restore(im);
        signal(elpptr->hsem);
        return len;
    }

    /* Ensure there is enough buffer space */
    if (elpptr->count >= ELOOP_NBUF)
    {
        restore(im);
        return SYSERR;
    }

    index = (elpptr->count + elpptr->index) % ELOOP_NBUF;

    /* Add to buffer */
    elpptr->buffer[index] = pkt;
    elpptr->pktlen[index] = len;
    elpptr->count++;

    /* Increment count of packets written */
    elpptr->nout++;

    restore(im);

    signal(elpptr->sem);

    return len;
}
