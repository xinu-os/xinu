/**
 * @file tcpSendSyn.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Sends an SYN packet to establish a TCP connection.
 * @param tcpptr pointer to the transmission control block for connection
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpSendSyn(struct tcb *tcbptr)
{
    uchar control;
    control = TCP_CTRL_SYN;

    if (tcbptr->sndflg & TCP_FLG_SNDACK)
    {
        tcbptr->sndflg &= ~TCP_FLG_SNDACK;
        control |= TCP_CTRL_ACK;
    }

    if (SYSERR == tcpSend(tcbptr, control, tcbptr->sndnxt,
                          tcbptr->rcvnxt, NULL, 1))
    {
        return SYSERR;
    }

    tcbptr->sndnxt++;
    tcbptr->sndflg |= TCP_FLG_SYN;

    if (SYSERR == tcpTimerSched(tcbptr->rxttime, tcbptr, TCP_EVT_RXT))
    {
        return SYSERR;
    }

    return OK;
}
