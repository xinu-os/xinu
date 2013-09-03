/**
 * @file     rtpRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <string.h>
#include <interrupt.h>
#include <ipv4.h>
#include <icmp.h>
#include <rtp.h>

/**
 * Receive a RTP packet and place it in the RTP device's input buffer
 * @param pkt Incoming RTP packet
 * @param src Source address
 * @param dst Destination address
 * @return OK if RTP packet is received properly, otherwise SYSERR
 */
syscall rtpRecv(struct packet *pkt, struct netaddr *src,
                struct netaddr *dst)
{
    struct rtpPkt *rtppkt;
    struct rtpPktExt *rtpext;
    struct rtp *rtpptr;
    struct rtpPkt *tpkt;
//#ifdef TRACE_RTP
    //char strA[20];
    //char strB[20];
//#endif                          /* TRACE_RTP */

    irqmask im;

    /* Point to the start of the RTP header */
    rtppkt = (struct rtpPkt *)pkt->curr;

    if (NULL == rtppkt)
    {
        //RTP_TRACE("Invalid RTP packet.");
        netFreebuf(pkt);
        return SYSERR;
    }
//TODO handle rtcp intake

    im = disable();

    //if (NULL == rtpptr)
    //{
//#ifdef TRACE_RTP
    //RTP_TRACE("No RTP socket found for this RTP packet.");
    //netaddrsprintf(strA, src);
    //netaddrsprintf(strB, dst);
    //RTP_TRACE("Source: %s:%d, Destination: %s:%d", strA,
    //          rtppkt->srcPort, strB, rtppkt->dstPort);
//#endif                          /* TRACE_RTP */
    //restore(im);
    /* Send ICMP port unreachable message */
    //icmpDestUnreach(pkt, ICMP_PORT_UNR);
    //netFreebuf(pkt);
    //return SYSERR;
    //}
    if (rtpptr->icount >= RTP_MAX_PKTS)
    {
        //RTP_TRACE("RTP buffer is full. Dropping RTP packet.");
        restore(im);
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Get some buffer space to store the packet */
    tpkt = rtpGetbuf(rtpptr);

    if (SYSERR == (int)tpkt)
    {
        //RTP_TRACE("Unable to get RTP buffer from pool. Dropping packet.");
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Copy the data of the packet into the input buffer at the current 
     * position */
    rtpext = (struct rtpPktExt *)tpkt;
    //TODO Fill out RTP Extension Packet

    memcpy((rtpext + 1), rtppkt, rtppkt->len);

    /* Store the temporary RTP packet in a FIFO buffer */
    rtpptr->in[(rtpptr->istart + rtpptr->icount) % RTP_MAX_PKTS] = tpkt;
    rtpptr->icount++;

    restore(im);

    signal(rtpptr->isem);

    netFreebuf(pkt);

    return OK;
}
