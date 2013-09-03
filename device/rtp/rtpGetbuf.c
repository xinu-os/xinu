/**
 * @file     rtpGetbuf.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <stdlib.h>
#include <rtp.h>

struct rtpPkt *rtpGetbuf(struct rtp *rtpptr)
{
    struct rtpPkt *rtppkt = NULL;

    rtppkt = bufget(rtpptr->inPool);
    if (SYSERR == (int)rtppkt)
    {
        return (struct rtpPkt *)SYSERR;
    }

    bzero(rtppkt, NET_MAX_PKTLEN);

    return rtppkt;
}
