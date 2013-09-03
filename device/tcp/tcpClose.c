/**
 * @file tcpClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <stdlib.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Close a TCP device.
 * @param devptr TCP device table entry
 * @return OK if TCP is closed properly, otherwise SYSERR
 */
devcall tcpClose(device *devptr)
{
    struct tcb *tcbptr;

    /* Setup and error check pointers to structures */
    tcbptr = &tcptab[devptr->minor];

    wait(tcbptr->mutex);
    switch (tcbptr->state)
    {
    case TCP_CLOSED:
        /* ERROR: connection does not exist */
        tcbptr->devstate = TCP_FREE;
        signal(tcbptr->mutex);
        return SYSERR;
    case TCP_SYNSENT:
        /* Return any outstanding writers with error */
        if (semcount(tcbptr->writers) < 0)
        {
            signaln(tcbptr->writers, semcount(tcbptr->writers) * -1);
        }
    case TCP_LISTEN:
        /* Return any outstanding readers with error */
        if (semcount(tcbptr->readers) < 0)
        {
            signaln(tcbptr->readers, semcount(tcbptr->readers) * -1);
        }
        tcpFree(tcbptr);
        break;
    case TCP_SYNRECV:
        tcbptr->sndfin = seqadd(tcbptr->snduna, tcbptr->ocount);
        tcbptr->sndflg |= TCP_FLG_FIN;
        if (0 == tcbptr->ocount)
        {
            tcbptr->sndflg |= TCP_FLG_SNDDATA;
            tcbptr->state = TCP_FINWT1;
        }
        break;
    case TCP_ESTAB:
        tcbptr->sndfin = seqadd(tcbptr->snduna, tcbptr->ocount);
        tcbptr->sndflg |= TCP_FLG_FIN;
        tcbptr->sndflg |= TCP_FLG_SNDDATA;
        tcbptr->state = TCP_FINWT1;
        break;
    case TCP_CLOSEWT:
        tcbptr->sndfin = seqadd(tcbptr->snduna, tcbptr->ocount);
        tcbptr->sndflg |= TCP_FLG_FIN;
        tcbptr->sndflg |= TCP_FLG_SNDDATA;
        tcbptr->state = TCP_LASTACK;
        break;
    case TCP_FINWT1:
    case TCP_FINWT2:
    case TCP_CLOSING:
    case TCP_LASTACK:
    case TCP_TIMEWT:
        /* ERROR: connection closing */
        signal(tcbptr->mutex);
        return SYSERR;
    }

    if (tcbptr->sndflg & TCP_FLG_SNDDATA)
    {
        tcpSendData(tcbptr);
    }

    signal(tcbptr->mutex);
    wait(tcbptr->openclose);


    return OK;
}
