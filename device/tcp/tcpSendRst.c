/**
 * @file tcpSendRst.c
 * @provides tcpSendRst
 *
 * $Id: tcpSendRst.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>
#include <tcp.h>

/**
 * Sends a reset in response to an incorrect TCP packet
 * @param pkt incoming packet which requires a reset response
 * @param src source IP address of the packet	
 * @param dst destination IP address of the packet	
 */
int tcpSendRst(struct packet *pkt, struct netaddr *src,
               struct netaddr *dst)
{
    struct packet *out;
    struct tcpPkt *tcp;
    struct tcpPkt *outtcp;
    ushort datalen;
    int result;
    ushort tcplen;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;
    tcplen = pkt->len - (pkt->curr - pkt->linkhdr);

    /* Verify not sending a reset in response to a reset */
    if (tcp->control & TCP_CTRL_RST)
    {
        return OK;
    }

    /* Get space to construct packet */
    out = netGetbuf();
    if (SYSERR == (int)out)
    {
        return SYSERR;
    }
    out->curr -= TCP_HDR_LEN;
    out->len += TCP_HDR_LEN;

    /* Set TCP header fields */
    outtcp = (struct tcpPkt *)out->curr;
    outtcp->srcpt = tcp->dstpt;
    outtcp->dstpt = tcp->srcpt;
    outtcp->offset = octets2offset(TCP_HDR_LEN);
    outtcp->control = TCP_CTRL_RST;
    if (tcp->control & TCP_CTRL_ACK)
    {
        outtcp->seqnum = tcp->acknum;
    }
    else
    {
        outtcp->seqnum = 0;
        outtcp->control |= TCP_CTRL_ACK;
    }
    datalen = tcplen - offset2octets(tcp->offset);
    if (tcp->control & TCP_CTRL_SYN)
    {
        datalen++;
    }
    if (tcp->control & TCP_CTRL_FIN)
    {
        datalen++;
    }
    outtcp->acknum = tcp->seqnum + datalen;

    /* Convert TCP header fields to net order */
    outtcp->srcpt = hs2net(outtcp->srcpt);
    outtcp->dstpt = hs2net(outtcp->dstpt);
    outtcp->seqnum = hl2net(outtcp->seqnum);
    outtcp->acknum = hl2net(outtcp->acknum);
    outtcp->window = hs2net(outtcp->window);
    outtcp->urgent = hs2net(outtcp->urgent);

    /* Calculate TCP checksum */
    outtcp->chksum = tcpChksum(out, TCP_HDR_LEN, src, dst);

    /* Send TCP packet */
    result = ipv4Send(out, src, dst, IPv4_PROTO_TCP);

    if (SYSERR == netFreebuf(out))
    {
        return SYSERR;
    }

    return result;
}
