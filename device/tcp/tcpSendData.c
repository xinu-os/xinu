/**
 * @file tcpSendData.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Sends pending outbound data (including SYN and FIN) for a TCP connection, 
 * if new data is ready for transmission.
 * @param tcpptr pointer to the transmission control block for connection
 * @return number of octets sent
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpSendData(struct tcb *tcbptr)
{
    uint wndused;      /**< amount of window filled with data pending ACK */
    uint pending;      /**< amount of data pending ACK or transmission */
    uint tosend;
    uint sent;
    uchar ctrl;

    /* Verify sender MSS is greater than 0 */
    if (tcbptr->sndmss == 0)
    {
        return SYSERR;
    }

    /* If FIN was already sent, then cannot send more data */
    if ((tcbptr->sndflg & TCP_FLG_FIN)
        && seqlt(tcbptr->sndfin, tcbptr->sndnxt))
    {
        return SYSERR;
    }

    /* If in retransmission or persist, cannot send data */
    if ((tcbptr->rxtcount > 0) || (tcbptr->sndflg & TCP_FLG_PERSIST))
    {
        return 0;
    }

    /* If the send window is zero, schedule a persist event */
    if (0 == tcbptr->sndwnd)
    {
        tcbptr->sndflg |= TCP_FLG_PERSIST;
        tcbptr->psttime = TCP_PST_INITTIME;
        tcpTimerSched(tcbptr->psttime, tcbptr, TCP_EVT_PERSIST);
        return 0;
    }

    /* Check if new transmssion is allowed */
    /* If (SNDNXT >= SNDUNA + SNDWND), then can't send data */
    if (seqlte(seqadd(tcbptr->snduna, tcbptr->sndwnd), tcbptr->sndnxt))
    {
        return 0;
    }

    /* Check if there is data to send */
    pending = tcbptr->ocount;
    if (tcbptr->sndflg & TCP_FLG_FIN)
    {
        pending++;
    }
    wndused = tcpSeqdiff(tcbptr->sndnxt, tcbptr->snduna);
    if (pending == wndused)
    {
        if (seqlt(tcbptr->snduna, tcbptr->sndnxt)
            && (tcpTimerRemain(tcbptr, TCP_EVT_RXT) <= 0))
        {
            tcpTimerSched(tcbptr->rxttime, tcbptr, TCP_EVT_RXT);
        }
        return 0;
    }

    /* There is data to send and space in the window to send it */
    ctrl = TCP_CTRL_ACK;
    /* Determine how much data to send */
    if (pending > tcbptr->sndwnd)
    {
        tosend = tcbptr->sndwnd - wndused;
    }
    else
    {
        tosend = pending - wndused;
        if (tcbptr->sndflg & TCP_FLG_FIN)
        {
            ctrl |= TCP_CTRL_FIN;
        }
    }

    /* Send as many maximum size segments as possible */
    sent = 0;
    while (tosend > tcbptr->sndmss)
    {
        tcpSend(tcbptr, TCP_CTRL_ACK, tcbptr->sndnxt, tcbptr->rcvnxt,
                (tcbptr->ostart + wndused) % TCP_OBLEN, tcbptr->sndmss);
        tosend -= tcbptr->sndmss;
        sent += tcbptr->sndmss;
        wndused += tcbptr->sndmss;
        tcbptr->sndnxt = seqadd(tcbptr->sndnxt, tcbptr->sndmss);
    }

    /* Send the remainder of the sendable data */
    tcpSend(tcbptr, ctrl, tcbptr->sndnxt, tcbptr->rcvnxt,
            (tcbptr->ostart + wndused) % TCP_OBLEN, tosend);
    sent += tosend;
    wndused += tosend;
    tcbptr->sndnxt = seqadd(tcbptr->sndnxt, tosend);

    /* If one does not already exist, schedule a retransmission event */
    if (tcpTimerRemain(tcbptr, TCP_EVT_RXT) <= 0)
    {
        tcpTimerSched(tcbptr->rxttime, tcbptr, TCP_EVT_RXT);
    }

    /* No longer need to ACK if data was sent, since ACK was piggybacked */
    if (sent > 0)
    {
        tcbptr->sndflg &= ~TCP_FLG_SNDACK;
    }

    tcbptr->sndflg &= ~TCP_FLG_SNDDATA;
    return sent;
}
