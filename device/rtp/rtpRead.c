/**
 * @file     rtpRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <rtp.h>

/**
 * Read a packet's data and put that data into the user buffer
 * @param devptr RTP device table entry
 * @param buf User buffer
 * @param len Length of data to be read and put into user buffer
 * @return OK if data read completes properly, otherwise SYSERR
 */
devcall rtpRead(device *devptr, void *buf, uint len)
{
    struct rtp *rtpptr = NULL;
    struct rtpPkt *rtppkt = NULL;
    struct rtpPktExt *rtppktext = NULL;
    uchar *buffer = buf;
    uchar *data = NULL;
    int count = 0;
    irqmask im;

    rtpptr = &rtptab[devptr->minor];

    im = disable();

    restore(im);
    wait(rtpptr->isem);
    im = disable();

    /* Get a pointer to the stored packet in the current position */
    //rtppktext = (struct rtpPktExt *)rtpptr->in[rtpptr->istart];
    rtppkt = (struct rtpPkt *)(rtppktext + 1);

    /* Make sure the packet is not NULL */
    if (NULL == rtppkt)
    {
        restore(im);
        return SYSERR;
    }

    /* Increment the start value to preserve the circular buffer */
    rtpptr->istart = (rtpptr->istart + 1) % RTP_MAX_PKTS;

    /* Decrement the count value before removing the packet from the buffer */
    rtpptr->icount--;

    count = rtppkt->len - RTP_HDR_LEN;
    data = rtppkt->data;

    restore(im);
    if (count > len)
    {
        count = len;
    }
    memcpy(buffer, data, count);

    /* Free the packet buffer */
    rtpFreebuf(rtppkt);

    return count;
}
