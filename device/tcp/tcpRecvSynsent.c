/**
 * @file tcpRecv.c
 * @provides tcpRecv
 *
 * $Id: tcpRecvSynsent.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * Processes an incoming packet for a TCP connection in the SYNSENT state.
 * Function based on RFC 763, pg 66-68.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @return OK or TCP_ERR_SNDRST
 * @pre-condition TCB mutex is already held
 */
int tcpRecvSynsent(struct packet *pkt, struct tcb *tcbptr)
{
    struct tcpPkt *tcp;
    bool ackAccept = FALSE;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;

    /* Process ACK */
    if (tcp->control & TCP_CTRL_ACK)
    {
        TCP_TRACE("ACK");
        /* If ACK is unacceptable send reset */
        if (seqlte(tcp->acknum, tcbptr->iss)
            || seqlt(tcbptr->sndnxt, tcp->acknum))
        {
            if (!(tcp->control & TCP_CTRL_RST))
            {
                tcbptr->sndflg |= TCP_FLG_SNDRST;
                return OK;
            }
            return OK;
        }

        /* Check if ACK is acceptable */
        if (seqlte(tcbptr->snduna, tcp->acknum)
            && seqlte(tcp->acknum, tcbptr->sndnxt))
        {
            ackAccept = TRUE;
        }
    }

    /* Process RST */
    if (tcp->control & TCP_CTRL_RST)
    {
        TCP_TRACE("RST");
        if (ackAccept)
        {
            /* ERROR: Connection reset */
            return TCP_ERR_RESET;
        }
        return OK;
    }

    /* Process SYN */
    if (tcp->control & TCP_CTRL_SYN)
    {
        TCP_TRACE("SYN");
        tcbptr->sndwnd = tcp->window;
        tcbptr->sndwl1 = tcp->seqnum;
        tcbptr->rcvnxt = seqadd(tcp->seqnum, 1);
        tcbptr->rcvwnd = seqadd(tcp->seqnum, TCP_INIT_WND);
        tcbptr->rcvflg |= TCP_FLG_SYN;
        tcbptr->sndflg |= TCP_FLG_SNDACK;
        if (ackAccept)
        {
            tcbptr->snduna = tcp->acknum;
            tcbptr->sndwl2 = tcp->acknum;
        }

        /* Remove any segments from retransmission queue which are ACKed */
        tcbptr->rxtcount = 0;
        tcpTimerPurge(tcbptr, TCP_EVT_RXT);
        /* If unacknowledged data remains, reschedule retransmit timer */
        if (seqlt(tcbptr->snduna, tcbptr->sndnxt))
        {
            tcpTimerSched(tcbptr->rxttime, tcbptr, TCP_EVT_RXT);
        }

        if (seqlt(tcbptr->iss, tcbptr->snduna))
        {
            tcbptr->state = TCP_ESTAB;
            signal(tcbptr->openclose);  /* Signal connection open */
            tcbptr->sndflg |= TCP_FLG_SNDDATA;
            return tcpRecvData(pkt, tcbptr);
        }
        else
        {
            tcbptr->state = TCP_SYNRECV;
            /* This send is like a retransmission */
            tcpSend(tcbptr, TCP_CTRL_SYN | TCP_CTRL_ACK,
                    tcbptr->iss, tcbptr->rcvnxt, NULL, 1);
            return tcpRecvData(pkt, tcbptr);
        }
    }

    return OK;
}
