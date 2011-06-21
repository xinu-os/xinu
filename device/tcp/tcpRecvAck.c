/**
 * @file tcpRecvAck.c
 * @provides tcpRecvAck
 *
 * $Id: tcpRecvAck.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Process an ackowledgement of data in an incoming TCP segment for a
 * connection which has been fully established.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @precondition TCB mutex is already held 
 * @postcondition TCB mutex is still held
 */
int tcpRecvAck(struct packet *pkt, struct tcb *tcbptr)
{
    uint amt = 0;
    tcpseq oldend, newend;
    struct tcpPkt *tcp;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;

    if (seqlt(tcbptr->snduna, tcp->acknum)
        && seqlte(tcp->acknum, tcbptr->sndnxt))
    {
        /* Calculate the amount of acknowledged data */
        amt = tcpSeqdiff(tcp->acknum, tcbptr->snduna);

        /* If the SYN is part of the ACK, don't include in data count */
        if ((tcbptr->sndflg & TCP_FLG_SYN)
            && (tcbptr->snduna == tcbptr->iss))
        {
            amt--;
        }

        /* If the FIN is part of the ACK, don't include in data count */
        if ((tcbptr->sndflg & TCP_FLG_FIN)
            && (seqlt(tcbptr->sndfin, tcp->acknum)))
        {
            amt--;
        }

        /* Adjust send buffer */
        tcbptr->ostart = (tcbptr->ostart + amt) % TCP_OBLEN;
        tcbptr->ocount -= amt;
        tcbptr->obytes += amt;
        if (tcbptr->ocount < TCP_OBLEN)
        {
            signal(tcbptr->writers);
        }

        tcbptr->snduna = tcp->acknum;

        /* Remove any segments from retransmission queue which are ACKed */
        tcbptr->rxtcount = 0;
        tcpRecvRtt(tcbptr);
        /* If unacknowledged data remains, reschedule retransmit timer */
        if (seqlt(tcbptr->snduna, tcbptr->sndnxt))
        {
            tcpTimerSched(tcbptr->rxttime, tcbptr, TCP_EVT_RXT);
        }

        tcbptr->sndflg |= TCP_FLG_SNDDATA;
    }

    /* Update send window (if packet is not out of order) */
    if (seqlt(tcbptr->sndwl1, tcp->seqnum)
        || ((tcbptr->sndwl1 == tcp->seqnum)
            && seqlte(tcbptr->sndwl2, tcp->acknum)))
    {
        /* Calculate sequence number for end of old and new send window */
        oldend = seqadd(tcbptr->sndwl2, tcbptr->sndwnd);
        newend = seqadd(tcp->acknum, tcp->window);

        tcbptr->sndwnd = tcp->window;
        tcbptr->sndwl1 = tcp->seqnum;
        tcbptr->sndwl2 = tcp->acknum;

        /* If window increased, clear persist event */
        if (!seqlte(newend, oldend))
        {
            tcpTimerPurge(tcbptr, TCP_EVT_PERSIST);
            tcbptr->sndflg &= ~TCP_FLG_PERSIST;
            tcbptr->sndflg |= TCP_FLG_SNDDATA;
        }
    }

    /* Send ACK in response to ACK of something not yet sent */
    if (seqlt(tcbptr->sndnxt, tcp->acknum))
    {
        tcbptr->sndflg |= TCP_FLG_SNDACK;
        return OK;
    }

    /* Ignore duplicate ACK */
    return OK;
}
