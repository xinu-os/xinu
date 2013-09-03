/**
 * @file tcpRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <semaphore.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Processes an incoming TCP packet.  See RFC 793, pg 65 for an overview 
 * of the TCP packet arrival algorithm.
 * @param pkt incoming packet
 * @param src source of incoming packet
 * @param dst destination of incoming packet
 */
int tcpRecv(struct packet *pkt, struct netaddr *src, struct netaddr *dst)
{
    struct tcpPkt *tcp;
    struct tcb *tcbptr;
    ushort tcplen;
    int result = 0;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;
    tcplen = pkt->len - (pkt->curr - pkt->linkhdr);

    /* Verify TCP checksum is correct */
    if (tcpChksum(pkt, tcplen, src, dst))
    {
        netFreebuf(pkt);
        TCP_TRACE("Bad Checksum");
        return OK;
    }

    /* Convert TCP header fields to host order */
    tcp->srcpt = net2hs(tcp->srcpt);
    tcp->dstpt = net2hs(tcp->dstpt);
    tcp->seqnum = net2hl(tcp->seqnum);
    tcp->acknum = net2hl(tcp->acknum);
    tcp->window = net2hs(tcp->window);
    tcp->urgent = net2hs(tcp->urgent);

    TCP_TRACE("RECV <C=0x%02X><S=%u><A=%u><dl=%u><w=%u>", tcp->control,
              tcp->seqnum, tcp->acknum, tcpSeglen(tcp, tcplen),
              tcp->window);

    /* Locate the TCP socket for the TCP packet */
    tcbptr = tcpDemux(tcp->dstpt, tcp->srcpt, dst, src);

    /* Send a reset if no matching stream socket was found */
    if (NULL == tcbptr)
    {
        tcpSendRst(pkt, src, dst);
        return netFreebuf(pkt);
    }

    /* Acquire mutex */
    wait(tcbptr->mutex);

    /* Verify the connection still exists, otherwise send a reset */
    if (TCP_CLOSED == tcbptr->state)
    {
        tcpSendRst(pkt, src, dst);
        signal(tcbptr->mutex);
        return netFreebuf(pkt);
    }

    tcpRecvOpts(pkt, tcbptr);

    /* Call appropriate receive function based on connection state */
    switch (tcbptr->state)
    {
    case TCP_LISTEN:
        result = tcpRecvListen(pkt, tcbptr, src);
        break;
    case TCP_SYNSENT:
        result = tcpRecvSynsent(pkt, tcbptr);
        break;
    case TCP_SYNRECV:
    case TCP_ESTAB:
    case TCP_FINWT1:
    case TCP_FINWT2:
    case TCP_CLOSEWT:
    case TCP_CLOSING:
    case TCP_LASTACK:
    case TCP_TIMEWT:
        result = tcpRecvOther(pkt, tcbptr);
        break;
    }

    /* Send a reset if necessary */
    if (tcbptr->sndflg & TCP_FLG_SNDRST)
    {
        tcpSendRst(pkt, src, dst);
    }

    /* If connection was reset, close the connection */
    if (TCP_ERR_RESET == result)
    {
        if (SYSERR == netFreebuf(pkt))
        {
            return SYSERR;
        }
        return tcpFree(tcbptr);
    }

    signal(tcbptr->mutex);

    if (SYSERR == netFreebuf(pkt))
    {
        return SYSERR;
    }
    return result;
}
