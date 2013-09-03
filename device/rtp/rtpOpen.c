/**
 * @file     rtpOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <bufpool.h>
#include <device.h>
#include <network.h>
#include <rtp.h>
#include <stdarg.h>
#include <interrupt.h>
#include <udp.h>

static ushort allocPort(void);

/**
 * Associate a RTP socket with a hardware device.
 * @param devptr RTP device table entry
 * @return OK if RTP is opened properly, otherwise SYSERR
 */
devcall rtpOpen(device *devptr, va_list ap)
{
    struct rtp *rtpptr = NULL;
    //Todo Attach to UDP Dev
    rtpptr->sequence = 0;       // Random number        
    rtpptr->tp = 0;
    rtpptr->tc = 0;
    rtpptr->senders = 0;
    rtpptr->pmembers = 0;
    rtpptr->members = 1;
    rtpptr->we_sent = false;
    //rtpptr->rtcp_bw = //specified fraction of session bandwidth
    rtpptr->inital = true;
    //rtpptr->avg_rtcp_size = (1/16) * //packet_size + (15/16) * rtpptr->avg_rtcp_size;
    irqmask im;

    rtpptr = &rtptab[devptr->minor];

    im = disable();
    /* Check if RTP is already open */
    if (RTP_OPEN == rtpptr->state)
    {
        RTP_TRACE("rtp%d has already been opened.", devptr->minor);
        restore(im);
        return SYSERR;
    }

    rtpptr->state = RTP_OPEN;
    rtpptr->dev = devptr;

    /* Initialize incoming packet buffer */
    rtpptr->icount = 0;
    rtpptr->istart = 0;

    /* Initialize the semaphore */
    rtpptr->isem = semcreate(0);

    /* Parse arguments */

    /* Allocate received RTP packet buffer pool */
    rtpptr->inPool = bfpalloc(NET_MAX_PKTLEN, RTP_MAX_PKTS);
    RTP_TRACE("rtp%d inPool has been assigned pool ID %d.\r\n",
              devptr->minor, rtpptr->inPool);

    restore(im);
    return OK;
}

/**
 * Allocates an unused RTP port to use as a local port
 * @return RTP port
 */
static ushort allocPort()
{
    static ushort nextport = 0;
    int i = 0;

    nextport = (nextport + 1) % (RTP_PMAX - RTP_PSTART);
    for (i = 0; i < NRTP; i++)
    {
        if ((rtptab[i].localpt == (nextport + RTP_PSTART))
            || (RTP_PORT_TRACEROUTE == (nextport + RTP_PSTART)))
        {
            nextport = (nextport + 1) % (RTP_PMAX - RTP_PSTART);
            i = 0;
        }
    }

    return (nextport + RTP_PSTART);
}
