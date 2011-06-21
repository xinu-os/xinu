/**
 * @file tcpSendRxt.c
 * @provides tcpSendRxt
 *
 * $Id: tcpSendRxt.c 2039 2009-08-14 22:55:10Z agember $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Retransmitts a segment of pending outbound data (including SYN and FIN) 
 * for a TCP connection.
 * @param tcpptr pointer to the transmission control block for connection
 * @return number of octets sent
 */
int tcpSendRxt(struct tcb *tcbptr)
{
    uint pending;      /**< amount of data pending ACK */
    uint tosend;
    uchar control = NULL;
    int time;
    bool first = FALSE;

    wait(tcbptr->mutex);

    /* Verify there is data to retransmit and not in persist output state */
    if ((!seqlt(tcbptr->snduna, tcbptr->sndnxt))
        || (tcbptr->sndflg & TCP_FLG_PERSIST))
    {
        signal(tcbptr->mutex);
        return SYSERR;
    }

    /* Determine if this is the first retransmission */
    if (0 == tcbptr->rxtcount)
    {
        first = TRUE;
    }

    /* Increment the count of retransmissions and abort connection 
     * if maximum numer of retransmissions is reached */
    tcbptr->rxtcount++;
    if (tcbptr->rxtcount > TCP_RXT_MAXCOUNT)
    {
        tcpFree(tcbptr);
        return 0;
    }

    /* Reschedule retransmit event */
    time = tcbptr->rxttime << tcbptr->rxtcount;
    if (time > TCP_RXT_MAXTIME)
    {
        time = TCP_RXT_MAXTIME;
    }
    tcpTimerSched(time, tcbptr, TCP_EVT_RXT);

    /* Check if the SYN needs to be resent */
    if ((tcbptr->sndflg & TCP_FLG_SYN) && (tcbptr->snduna == tcbptr->iss))
    {
        control = TCP_CTRL_SYN;
        /* Also include ACK if SYN from other side has been recevied */
        if (tcbptr->rcvflg & TCP_FLG_SYN)
        {
            control |= TCP_CTRL_ACK;
        }
        /* Retransmit SYN */
        tcpSend(tcbptr, control, tcbptr->snduna, tcbptr->rcvnxt, 0, 1);

        signal(tcbptr->mutex);
        return 1;
    }

    /* Calculate amount of data pending ACK */
    pending = tcpSeqdiff(tcbptr->sndnxt, tcbptr->snduna);
    control = TCP_CTRL_ACK;

    /* Determine if FIN is pending ACK */
    if ((tcbptr->sndflg & TCP_FLG_FIN)
        && seqlte(tcbptr->snduna, tcbptr->sndfin)
        && seqlt(tcbptr->sndfin, tcbptr->sndnxt))
    {
        control |= TCP_CTRL_FIN;
    }

    /* Calculate amount of data to send */
    tosend = pending;
    if (pending > tcbptr->sndmss)
    {
        tosend = tcbptr->sndmss;
        control &= ~TCP_CTRL_FIN;
    }

    /* Send data */
    tcpSend(tcbptr, control, tcbptr->snduna, tcbptr->rcvnxt,
            tcbptr->ostart, tosend);

    /* Adjust sender congestion window */
    if (first)
    {
        tcbptr->sndsst = tcbptr->sndcwn;
    }
    if (tcbptr->sndwnd < tcbptr->sndsst)
    {
        tcbptr->sndsst = tcbptr->sndwnd >> 1;
    }
    else
    {
        tcbptr->sndsst = tcbptr->sndsst >> 1;
    }
    if (tcbptr->sndsst < tcbptr->sndmss)
    {
        tcbptr->sndsst = tcbptr->sndmss;
    }
    tcbptr->sndcwn = tcbptr->sndmss;

    signal(tcbptr->mutex);
    return tosend;
}
