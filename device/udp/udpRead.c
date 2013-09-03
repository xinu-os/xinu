/**
 * @file     udpRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <interrupt.h>
#include <string.h>
#include <udp.h>

/**
 * @ingroup udpexternal
 *
 * Read the next UDP packet from a UDP device and place its data into the
 * provided buffer.
 *
 * In the default mode, the resulting data will be the UDP payload.  If instead
 * the UDP is in @ref UDP_FLAG_PASSIVE "passive mode", the resulting data will
 * be the UDP pseudo-header, directly followed by the UDP header, directly
 * followed by the UDP payload.
 *
 * @param devptr
 *      Device table entry for the UDP device.
 * @param buf
 *      Buffer into which to read the data.
 * @param len
 *      Maximum amount of data to read (length of @p buf).
 *
 * @return
 *      On success, returns the number of bytes read.  This is normally the size
 *      of the UDP packet (see note about passive mode above), but as special
 *      cases it will be 0 if the UDP is in non-blocking mode and no packets are
 *      available, or it will be @p len if the actual amount of data that was
 *      available was greater than @p len.  Alternatively, if the UDP device was
 *      not initially open or was closed while attempting to read a packet,
 *      ::SYSERR is returned.
 */
devcall udpRead(device *devptr, void *buf, uint len)
{
    struct udp *udpptr;
    irqmask im;
    const struct udpPseudoHdr *pseudo;
    const struct udpPkt *udppkt;
    uint count;
    const void *data;

    udpptr = &udptab[devptr->minor];

    im = disable();

    /* Make sure the UDP device is open.  */
    if (UDP_OPEN != udpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* If the UDP device is in non-blocking mode, require that at least one UDP
     * packet be available without waiting.  */
    if ((udpptr->flags & UDP_FLAG_NOBLOCK) && (udpptr->icount < 1))
    {
        restore(im);
        return 0;
    }

    /* Wait for a UDP packet to be available.  */
    wait(udpptr->isem);

    /* Make sure the UDP device wasn't closed while waiting for a packet.  */
    if (UDP_OPEN != udpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* Get the next UDP packet from the circular buffer, then remove it.
     * Beware: normally it would be safe to restore interrupts after doing this,
     * but we need to prevent a race with udpClose().  */
    pseudo = (const struct udpPseudoHdr *)udpptr->in[udpptr->istart];
    udpptr->istart = (udpptr->istart + 1) % UDP_MAX_PKTS;
    udpptr->icount--;

    /* Set pointer to the UDP header, which directly follows the pseudo-header.
     */
    udppkt = (const struct udpPkt *)(pseudo + 1);

    /* Copy the UDP data into the caller's buffer.  As documented, the exact
     * data that's copied depends on the current mode of the UDP device.
     * Furthermore, be careful to copy at most the number of bytes the caller
     * requested.  */
    if (UDP_FLAG_PASSIVE & udpptr->flags)
    {
        count = udppkt->len + sizeof(struct udpPseudoHdr);
        data = pseudo;
    }
    else
    {
        count = udppkt->len - UDP_HDR_LEN;
        data = udppkt->data;
    }
    if (count > len)
    {
        count = len;
    }
    memcpy(buf, data, count);

    /* Free the packet buffer, restore interrupts, and return the number of
     * bytes read.  */
    udpFreebuf((struct udpPkt *)pseudo);
    restore(im);
    return count;
}
