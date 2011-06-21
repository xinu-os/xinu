/**
 * @file     udpRead.c
 * @provides udpRead
 *
 * $Id: udpRead.c 2083 2009-10-06 19:53:29Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <udp.h>

/**
 * Read a packet's data and put that data into the user buffer
 * @param devptr UDP device table entry
 * @param buf User buffer
 * @param len Length of data to be read and put into user buffer
 * @return OK if data read completes properly, otherwise SYSERR
 */
devcall udpRead(device *devptr, void *buf, uint len)
{
    struct udp *udpptr;
    struct udpPkt *udppkt;
    uchar *buffer = buf;
    uchar *data;
    int count = 0;
    irqmask im;

    udpptr = &udptab[devptr->minor];

    im = disable();

    if ((udpptr->flags & UDP_FLAG_NOBLOCK) && (udpptr->icount < 1))
    {
        restore(im);
        return 0;
    }

    restore(im);
    wait(udpptr->isem);
    im = disable();

    /* Get a pointer to the stored packet in the current position */
    udppkt = udpptr->in[udpptr->istart];

    /* Make sure the packet is not NULL */
    if (NULL == udppkt)
    {
        restore(im);
        return SYSERR;
    }

    /* Increment the start value to preserve the circular buffer */
    udpptr->istart = (udpptr->istart + 1) % UDP_MAX_PKTS;

    /* Decrement the count value before removing the packet from the buffer */
    udpptr->icount--;

    /* Put the UDP packet's data in the user's buffer */
    if (UDP_FLAG_INCHDR & udpptr->flags)
    {
        count = udppkt->len;
        data = (uchar *)udppkt;
    }
    else
    {
        count = udppkt->len - UDP_HDR_LEN;
        data = udppkt->data;
    }

    restore(im);
    if (count > len)
    {
        count = len;
    }
    memcpy(buffer, data, count);

    /* Free the packet buffer */
    udpFreebuf(udppkt);

    return count;
}
