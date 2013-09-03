/**
 * @file     ethloopWrite.c
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
 * Write data to an Ethernet Loopback device.  On success, the data will be
 * available to be read by a subsequent call to ethloopRead().
 *
 * @param devptr
 *      Pointer to the device table entry for the ethloop.
 *
 * @param buf
 *      Buffer of data to write.
 *
 * @param len
 *      Length of data to write, in bytes.
 *
 * @return
 *      On success, returns the number of bytes written, which will be exactly
 *      @p len.  On failure, returns SYSERR.
 */
devcall ethloopWrite(device *devptr, const void *buf, uint len)
{
    struct ethloop *elpptr;
    irqmask im;
    int index;
    char *pkt;

    elpptr = &elooptab[devptr->minor];

    /* Make sure the packet isn't too small or too large  */
    if ((len < ELOOP_LINKHDRSIZE) || (len > ELOOP_BUFSIZE))
    {
        return SYSERR;
    }

    im = disable();

    /* Make sure the ethloop is actually open  */
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Drop packet if drop flags(s) are set */
    if (elpptr->flags & (ELOOP_FLAG_DROPNXT | ELOOP_FLAG_DROPALL))
    {
        elpptr->flags &= ~ELOOP_FLAG_DROPNXT;
        restore(im);
        return len;
    }

    /* Allocate buffer space.  This is blocking, so it can only fail if the pool
     * ID was corrupted.  */
    pkt = (char *)bufget(elpptr->poolid);
    if (SYSERR == (int)pkt)
    {
        restore(im);
        return SYSERR;
    }

    /* Copy supplied buffer into allocated buffer */
    memcpy(pkt, buf, len);

    /* Hold next packet if the appropriate flag is set */
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

    /* Ensure there is enough buffer space (there always should be)  */
    if (elpptr->count >= ELOOP_NBUF)
    {
        buffree(pkt);
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
