/**
 * @file tcpRecvOther.c
 * @provides tcpRecvOther
 *
 * $Id: tcpRecvOther.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * Processes an incoming packet for a TCP connection in the SYNRECV, ESTAB,
 * FINWT1, FINWT2, CLOSEWT, CLOSING, LASTACK, or TIMEWT state.
 * Function based on RFC 763, pg 69-73.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpRecvOther(struct packet *pkt, struct tcb *tcbptr)
{
    bool segAccept = FALSE;
    struct tcpPkt *tcp;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;

    /* Check if segment is acceptable */
    segAccept = tcpRecvValid(pkt, tcbptr);
    if (FALSE == segAccept)
    {
        /* If segment is unacceptable and RST bit is set, drop and return */
        if (tcp->control & TCP_CTRL_RST)
        {
            return OK;
        }
        /* If segement is unacceptable, ACK and drop */
        else
        {
            tcpSendAck(tcbptr);
            return OK;
        }
    }

    /* Process RST */
    if (tcp->control & TCP_CTRL_RST)
    {
        switch (tcbptr->state)
        {
        case TCP_SYNRECV:
            tcbptr->rxtcount = 0;
            tcpTimerPurge(tcbptr, TCP_EVT_RXT);
            if (TCP_PASSIVE == tcbptr->opentype)
            {
                tcbptr->state = TCP_LISTEN;
                return OK;
            }
            else
            {
                return TCP_ERR_RESET;
            }
        case TCP_ESTAB:
        case TCP_FINWT1:
        case TCP_FINWT2:
        case TCP_CLOSEWT:
        case TCP_CLOSING:
        case TCP_LASTACK:
        case TCP_TIMEWT:
            return TCP_ERR_RESET;
        }
    }

    /* Process SYN */
    if (tcp->control & TCP_CTRL_SYN)
    {
        /* SYN must be in the window, this is an error */
        tcbptr->sndflg |= TCP_FLG_SNDRST;
        return TCP_ERR_RESET;
    }

    /* ACK bit should be set, drop segment if not */
    if (!(tcp->control & TCP_CTRL_ACK))
    {
        return OK;
    }

    /* Process ACK bit */
    switch (tcbptr->state)
    {
    case TCP_SYNRECV:
        if (seqlte(tcbptr->snduna, tcp->acknum)
            && seqlte(tcp->acknum, tcbptr->sndnxt))
        {
            tcbptr->state = TCP_ESTAB;
            signal(tcbptr->openclose);  /* Signal connection open */
        }
        else
        {
            tcbptr->sndflg |= TCP_FLG_SNDRST;
            return OK;
        }
    case TCP_CLOSEWT:
    case TCP_ESTAB:
        tcpRecvAck(pkt, tcbptr);
        break;
    case TCP_FINWT1:
        tcpRecvAck(pkt, tcbptr);
        /* If FIN is now acknowledged, enter FINWT2 state */
        if (seqlte(tcbptr->sndfin, tcbptr->snduna))
        {
            signal(tcbptr->openclose);
            tcbptr->state = TCP_FINWT2;
        }
        break;
    case TCP_FINWT2:
        tcpRecvAck(pkt, tcbptr);
        /* If no retransmissions remain, acknowledge close as OK */
        if (tcpTimerRemain(tcbptr, TCP_EVT_RXT) <= 0)
        {
            signal(tcbptr->openclose);
        }
        break;
    case TCP_CLOSING:
        tcpRecvAck(pkt, tcbptr);
        /* If FIN is now acknowledged, enter TIMEWT state */
        if (seqlte(tcbptr->sndfin, tcbptr->snduna))
        {
            signal(tcbptr->openclose);
            tcbptr->state = TCP_FINWT2;
            tcpTimerSched(TCP_TWOMSL, tcbptr, TCP_EVT_TIMEWT);
        }
        break;
    case TCP_LASTACK:
        /* If FIN is now acknowledged, delete TCB and close */
        if (seqlte(tcbptr->sndfin, tcbptr->snduna))
        {
            signal(tcbptr->openclose);
            return TCP_ERR_RESET;
        }
        break;
    case TCP_TIMEWT:
        tcpTimerPurge(tcbptr, NULL);
        tcpTimerSched(TCP_TWOMSL, tcbptr, TCP_EVT_TIMEWT);
        break;
    }
    return tcpRecvData(pkt, tcbptr);
}
