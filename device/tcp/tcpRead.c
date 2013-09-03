/**
 * @file tcpRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

static int stateCheck(struct tcb *);

/**
 * @ingroup tcp
 *
 * Read into a buffer from TCP.
 * @param devptr TCP device table entry
 * @param buf buffer to read octets into
 * @param len size of the buffer
 * @return count of octets read
 */
devcall tcpRead(device *devptr, void *buf, uint len)
{
    int count = 0;
    struct tcb *tcbptr;
    int check;
    char *buffer = buf;

    tcbptr = &tcptab[devptr->minor];

    wait(tcbptr->mutex);

    /* Handle states for which no data will ever be received */
    check = stateCheck(tcbptr);
    if (check != OK)
    {
        return SYSERR;
//        return check; 
    }

    signal(tcbptr->mutex);

    /* Put each octet into the buffer from the input buffer */
    while (count < len)
    {
        /* Wait for input or FIN */
        wait(tcbptr->readers);
        wait(tcbptr->mutex);

        /* Return if changed to a state where no data will ever be recvd */
        check = stateCheck(tcbptr);
        if (check != OK)
        {
            return SYSERR;
//            return check; 
        }

        /* Read as much as possible from the input buffer 
         * Preserve the circular buffer  */
        while ((tcbptr->icount > 0) && (count < len))
        {
            *buffer++ = tcbptr->in[tcbptr->istart];
            tcbptr->imark[tcbptr->istart] = FALSE;
            tcbptr->istart = (tcbptr->istart + 1) % TCP_IBLEN;
            tcbptr->icount--;
            count++;
        }

#ifdef TCP_GRACIOUSACK
        /* Send gracious acknowledgement if window has increaed */
        if (seqlte(tcbptr->rcvwnd, tcbptr->rcvnxt))
        {
            if (tcpSendWindow(tcbptr) > 0)
            {
                tcpSendAck(tcbptr);
            }
        }
#endif

        /* If data remains, another reader can read */
        if ((tcbptr->icount > 0) && (semcount(tcbptr->readers) < 1))
        {
            signal(tcbptr->readers);
        }

        signal(tcbptr->mutex);
    }

    return count;
}

static int stateCheck(struct tcb *tcbptr)
{
    switch (tcbptr->state)
    {
    case TCP_CLOSED:
        /* No connection exists */
        signal(tcbptr->mutex);
        return TCP_ERR_NOCONN;
    case TCP_CLOSEWT:
        /* No more data will come, but satisfy with already recvd data */
        if (tcbptr->icount > 0)
        {
            return OK;
        }
        /* No already received data */
    case TCP_CLOSING:
    case TCP_LASTACK:
    case TCP_TIMEWT:
        /* Connection closing */
        signal(tcbptr->mutex);
        return TCP_ERR_CLOSING;
    }
    return OK;
}
