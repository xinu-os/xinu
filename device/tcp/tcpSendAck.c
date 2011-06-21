/**
 * @file tcpSendAck.c
 * @provides tcpSendAck
 *
 * $Id: tcpSendAck.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Sends an ACK packet to ackowledge all received data for a TCP connection.
 * @param tcpptr pointer to the transmission control block for connection
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpSendAck(struct tcb *tcbptr)
{
    tcbptr->sndflg &= ~TCP_FLG_SNDACK;
    return tcpSend(tcbptr, TCP_CTRL_ACK, tcbptr->sndnxt,
                   tcbptr->rcvnxt, 0, 0);
}
