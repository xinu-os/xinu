/**
 * @file icmpEchoRequest.c
 * @provides icmpEchoRequest
 *
 * $Id: icmpEchoRequest.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <icmp.h>
#include <stdlib.h>
#include <clock.h>
#include <interrupt.h>

/**
 * Send an ICMP Echo (Ping) Request.
 * @param dst destination address
 * @param id  ping stream identifier
 * @param seq sequence number
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall icmpEchoRequest(struct netaddr *dst, ushort id, ushort seq)
{
    struct packet *pkt = NULL;
    struct icmpEcho *echo = NULL;
    int result = OK;
    irqmask im;

    ICMP_TRACE("echo request(%d, %d)", id, seq);
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ICMP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    pkt->len = sizeof(struct icmpEcho);
    pkt->curr -= pkt->len;

    echo = (struct icmpEcho *)pkt->curr;
    echo->id = hs2net(id);
    echo->seq = hs2net(seq);
    /* Our optional data payload includes room for the departure */
    /*  and arrival timestamps, in seconds, milliseconds, and    */
    /*  clock cycles.                                            */
    im = disable();
    echo->timecyc = hl2net(clkcount());
    echo->timetic = hl2net(clkticks);
    echo->timesec = hl2net(clktime);
    restore(im);
    echo->arrivcyc = 0;
    echo->arrivtic = 0;
    echo->arrivsec = 0;

    ICMP_TRACE("Sending Echo Request id = %d, seq = %d, time = %d.%d",
               net2hs(echo->id), net2hs(echo->seq),
               net2hl(echo->timesec), net2hl(echo->timetic));

    result = icmpSend(pkt, ICMP_ECHO, 0, sizeof(struct icmpEcho), dst);
    netFreebuf(pkt);
    return result;
}
