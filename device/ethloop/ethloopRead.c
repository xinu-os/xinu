/**
 * @file     ethloopRead.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <interrupt.h>
#include <stddef.h>
#include <string.h>

/**
 * @ingroup ethloop
 *
 * Read data from an Ethernet Loopback device.  This will block until data has
 * been made available by a call to ethloopWrite().
 *
 * @param devptr
 *      Pointer to the device table entry for the ethloop.
 *
 * @param buf
 *      Buffer into which to place the read data.
 *
 * @param len
 *      Maximum length of the data to read, in bytes.
 *
 * @return
 *      On success, returns the number of bytes read, which will be less than or
 *      equal to @p len.  Otherwise returns SYSERR.
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
