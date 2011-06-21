/**
 * @file tcpWrite.c
 * @provides tcpWrite
 *
 * $Id: tcpWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

static int stateCheck(struct tcb *);

/**
 * Write into a buffer to send via TCP.
 * @param devptr TCP device table entry
 * @param buf buffer to read octets into
 * @param len size of the buffer
 * @return count of octets written
 */
devcall tcpWrite(device *devptr, void *buf, uint len)
{
    uint count = 0;
    uchar ch;
    struct tcb *tcbptr;
    uchar *buffer = buf;
    int check;

    tcbptr = &tcptab[devptr->minor];

    /* Handle states for which data can never be sent */
    wait(tcbptr->mutex);
    check = stateCheck(tcbptr);
    if (check != OK)
    {
        signal(tcbptr->mutex);
        return check;
    }
    signal(tcbptr->mutex);

    /* Put each octet from the buffer into output buffer */
    while (count < len)
    {
        /* Wait for space and write as much as possible into the output
         * buffer; Preserve the circular buffer */
        wait(tcbptr->writers);
        wait(tcbptr->mutex);

        /* Returned if changed to a state where no data can be sent */
        check = stateCheck(tcbptr);
        if (check != OK)
        {
            signal(tcbptr->writers);
            return check;
        }

        while ((tcbptr->ocount < TCP_OBLEN) && (count < len))
        {
            ch = *buffer++;
            tcbptr->out[((tcbptr->ostart + tcbptr->ocount) % TCP_OBLEN)] =
                ch;
            tcbptr->ocount++;
            count++;
        }
        /* If space remains, another writer can write */
        if (tcbptr->ocount < TCP_OBLEN)
        {
            signal(tcbptr->writers);
        }
        if ((TCP_ESTAB == tcbptr->state)
            || (TCP_CLOSEWT == tcbptr->state))
        {
            tcpSendData(tcbptr);
        }
        signal(tcbptr->mutex);
    }

    return count;
}

/*
 * Verifys the connection is an appropriate state
 * @param tcbptr TCB for connection
 * @return OK if connection is ready to send, otherwise error
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
static int stateCheck(struct tcb *tcbptr)
{
    switch (tcbptr->state)
    {
    case TCP_CLOSED:
        /* No connection exists */
        signal(tcbptr->mutex);
        return TCP_ERR_NOCONN;
    case TCP_LISTEN:
        /* If foreign socket is specified change to active connection */
        if ((tcbptr->remotept != NULL) && (tcbptr->remoteip.type != NULL))
        {
            tcbptr->state = TCP_SYNSENT;
            if (tcpOpenActive(tcbptr) != OK)
            {
                signal(tcbptr->mutex);
                return SYSERR;
            }
            /* Attempt to send SYN */
            if (tcbptr->sndflg & TCP_FLG_SYN)
            {
                tcpSendSyn(tcbptr);
            }
        }
        signal(tcbptr->mutex);
        return TCP_ERR_NOSPEC;
    case TCP_FINWT1:
    case TCP_FINWT2:
    case TCP_CLOSING:
    case TCP_LASTACK:
    case TCP_TIMEWT:
        /* Connection closing */
        signal(tcbptr->mutex);
        return TCP_ERR_CLOSING;
    }
    return OK;
}
