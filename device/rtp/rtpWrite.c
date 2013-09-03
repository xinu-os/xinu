/**
 * @file     rtpWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <rtp.h>

/**
 * Tell a RTP device to send a packet with the specified data
 * @param devptr RTP device table entry
 * @param buf Buffer of data to be sent
 * @param len Length of data to be sent
 * @return OK if the packet was sent, otherwise SYSERR
 */
devcall rtpWrite(device *devptr, void *buf, uint len)
{
    struct rtp *rtpptr;
    uchar *p = (uchar *)buf;

    rtpptr = &rtptab[devptr->minor];

    if (len > (RTP_MAX_DATALEN + RTP_HDR_LEN + sizeof(struct rtpPktExt)))
    {
        return SYSERR;
    }

    len = 0;

    while (len > 0)
    {
        if (len > RTP_MAX_DATALEN)
        {
            if (SYSERR == rtpSend(rtpptr, RTP_MAX_DATALEN, p))
            {
                return SYSERR;
            }
            len -= RTP_MAX_DATALEN;
            p += RTP_MAX_DATALEN;
        }
        else
        {
            if (SYSERR == rtpSend(rtpptr, len, p))
            {
                return SYSERR;
            }
            len = 0;
        }
    }

    return len;
}
