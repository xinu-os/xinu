/**
 * @file tcpSendPersist.c
 * @provides tcpSendPersist
 *
 * $Id: tcpSendPersist.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Send a segment to probe a receiver who advertised a window size of 0.
 * @param tcpptr pointer to the transmission control block for connection
 * @return number of octets sent
 */
int tcpSendPersist(struct tcb *tcbptr)
{
    uint tosend;

    wait(tcbptr->mutex);

    /* Verify there is data to transmit */
    if (tcbptr->ocount <= 0)
    {
        tcbptr->sndflg &= ~TCP_FLG_PERSIST;
        signal(tcbptr->mutex);
        return SYSERR;
    }

    /* Reschedule persist event */
    tcbptr->psttime = tcbptr->psttime << 1;
    if (tcbptr->psttime > TCP_PST_MAXTIME)
    {
        tcbptr->psttime = TCP_PST_MAXTIME;
    }
    tcpTimerSched(tcbptr->psttime, tcbptr, TCP_EVT_PERSIST);

    /* Send as much as possible */
    tosend = tcbptr->ocount;
    if (tosend > tcbptr->sndmss)
    {
        tosend = tcbptr->sndmss;
    }

    /* Send data */
    tcpSend(tcbptr, TCP_CTRL_ACK, tcbptr->snduna, tcbptr->rcvnxt,
            tcbptr->ostart, tosend);

    /* TODO: Determine if flags need to be cleared */
    signal(tcbptr->mutex);
    return tosend;
}
