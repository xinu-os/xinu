/**
 * @file     ethloopRead.c
 * @provides ethloopRead
 *
 * $Id: ethloopRead.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <interrupt.h>
#include <string.h>

/**
 * Read characters from an ethloop.
 * @param devptr 
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, SYSERR if an error occurs
 */
devcall ethloopRead(device *devptr, void *buf, uint len)
{
    struct ethloop *elpptr;
    irqmask im;
    char *pkt;
    int pktlen;

    elpptr = &elooptab[devptr->minor];

    im = disable();
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* wait until the buffer has a packet */
    wait(elpptr->sem);

    pkt = elpptr->buffer[elpptr->index];
    pktlen = elpptr->pktlen[elpptr->index];
    elpptr->buffer[elpptr->index] = NULL;
    elpptr->pktlen[elpptr->index] = 0;
    elpptr->count--;
    elpptr->index = (elpptr->index + 1) % ELOOP_NBUF;
    restore(im);

    if (len < pktlen)
    {
        pktlen = len;
    }

    memcpy(buf, pkt, pktlen);
    buffree(pkt);

    return pktlen;
}
