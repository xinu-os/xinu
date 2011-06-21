/**
 * @file     rawRead.c
 * @provides rawRead
 *
 * $Id: rawRead.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <interrupt.h>
#include <network.h>
#include <raw.h>
#include <string.h>

/**
 * Read a packet from a raw socket.
 * @param devptr pointer to RAW device
 * @param buf buffer for read packet
 * @param len size of the buffer 
 * @return number of octets read, SYSERR if error occurred 
 */
devcall rawRead(device *devptr, void *buf, uint len)
{
    struct raw *rawptr;
    struct packet *pkt;
    uchar *data;
    uint dlen;
    irqmask im;

    rawptr = &rawtab[devptr->minor];
    im = disable();
    if (RAW_ALLOC != rawptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Read next packet */
    wait(rawptr->isema);
    pkt = rawptr->in[rawptr->istart];
    RAW_TRACE("Read packet from pos %d", rawptr->istart);

    /* Bind remoteip if flag is set */
    if (rawptr->flags & RAW_IACCEPT)
    {
        netaddrcpy(&rawptr->remoteip, &rawptr->src[rawptr->istart]);
        rawptr->flags = rawptr->flags & ~RAW_IACCEPT;
#ifdef TRACE_RAW
        char str[20];
        netaddrsprintf(str, &rawptr->remoteip);
        RAW_TRACE("Accept remote IP %s", str);
#endif
    }

    /* Update buffer */
    rawptr->in[rawptr->istart] = NULL;
    rawptr->icount--;
    rawptr->istart = (rawptr->istart + 1) % RAW_IBLEN;
    restore(im);

    /* Determine what data to include */
    if (rawptr->flags & RAW_IHDR)
    {
        data = pkt->nethdr;
        dlen = pkt->len - (pkt->nethdr - pkt->linkhdr);
    }
    else
    {
        data = pkt->curr;
        dlen = pkt->len - (pkt->curr - pkt->linkhdr);
    }

    /* Set size of data to include */
    if (dlen > len)
    {
        dlen = len;
    }

    /* Copy payload into buffer */
    memcpy(buf, data, dlen);

    /* Free queued packet */
    if (SYSERR == netFreebuf(pkt))
    {
        RAW_TRACE("Failed to free buffer");
        return SYSERR;
    }

    return dlen;
}
