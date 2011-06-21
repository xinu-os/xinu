/**
 * @file tcpOpenActive.c
 * @provides tcpOpenActive, tcpIss
 *
 * $Id: tcpOpenActive.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Actively opens a TCP connection.
 * @param tcbptr TCB for connection
 * @return OK if tcp is opened properly, otherwise SYSERR
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
devcall tcpOpenActive(struct tcb *tcbptr)
{
    /* Verify connection is fully specified */
    if ((NULL == tcbptr->remotept) || (NULL == tcbptr->remoteip.type)
        || (NULL == tcbptr->localip.type))
    {
        return TCP_ERR_NOSPEC;
    }

    if (SYSERR == tcpSendSyn(tcbptr))
    {
        return SYSERR;
    }

    tcbptr->state = TCP_SYNSENT;

    return OK;
}
