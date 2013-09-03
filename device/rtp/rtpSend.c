/**
 * @file     rtpSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <ipv4.h>
#include <kernel.h>
#include <network.h>
#include <rtp.h>
#include <udp.h>

/**
 * Send a RTP packet across the network to a certain destination
 * @param rtpptr Pointer to RTP control block
 * @param datalen Length of data to be sent
 * @param buf Data to be sent
 * @return OK if packet is sent properly, otherwise SYSERR
 */
syscall rtpSend(struct rtp *rtpptr, ushort datalen, void *buf)
{
    struct packet *pkt;
    struct rtpPkt *rtppkt = NULL;
    struct rtpPktExt *rtppktext = NULL;
    int result;

    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        //RTP_TRACE("Failed to allocate buffer");
        return SYSERR;
    }

    /* Set the length of the packet and set the curr pointer back that 
     * length */
    pkt->len = datalen;
    /* Round the datalength to maintain word alignment */
    pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

    /* Copy packet plus header */
    memcpy(pkt->curr, (rtppktext + 1), datalen);
    rtppkt = (struct rtpPkt *)(pkt->curr);

    datalen += RTP_HDR_LEN;
    /* Set the length of the packet and set the curr pointer back that 
     * length */
    pkt->len = datalen;
    /* Round the datalength to maintain word alignment */
    pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

    /* Set RTP header fields and fill the packet with the data */
    //TODO
    rtppkt = (struct rtpPkt *)(pkt->curr);
    rtppkt->control = 0;
    rtppkt->seqNum = rtpptr->sequence;
    rtpptr->sequence++;
    rtppkt->timestamp =;
    rtppkt->ssrc =;
    rtppkt->csrc[0] =;

    memcpy(rtppkt->data, buf, datalen - RTP_HDR_LEN);

    /* Send the RTP packet through IP */
    //TODO result = udpSend(pkt, &localip, &remoteip, IPv4_PROTO_RTP);

    if (SYSERR == netFreebuf(pkt))
    {
        //RTP_TRACE("Failed to free buffer");
        return SYSERR;
    }

    return result;
}
