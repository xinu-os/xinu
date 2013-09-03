/**
 * @file rcpRecvRtt.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Handle round trip time estimates based on an incoming acknowledgement.
 * @param tcbptr pointer to transmission control block for connection
 * @return OK
 * @precondition TCB mutex is already held 
 * @postcondition TCB mutex is still held
 */
int tcpRecvRtt(struct tcb *tcbptr)
{
    int rtt, delta;

    rtt = tcpTimerPurge(tcbptr, TCP_EVT_RXT);
    if ((rtt != SYSERR) && (0 == tcbptr->rxtcount))
    {
        if (0 == tcbptr->sndrtt)
        {
            tcbptr->sndrtt = rtt << 3;
        }
        delta = rtt - (tcbptr->sndrtt >> 3);
        tcbptr->sndrtt += delta;
        delta = abs(delta);
        tcbptr->sndrtd += delta - (tcbptr->sndrtd >> 2);
        tcbptr->rxttime = ((tcbptr->sndrtt >> 2) + tcbptr->sndrtd) >> 1;
        if (tcbptr->rxttime < TCP_RXT_MINTIME)
        {
            tcbptr->rxttime = TCP_RXT_MINTIME;
        }
    }

    /* Handle slow start/congestion window increase */
    /* If the congestion window is less than the slow start threshold, increase
     * congestion window by MSS */
    if (tcbptr->sndcwn < tcbptr->sndsst)
    {
        tcbptr->sndcwn += tcbptr->sndmss;
    }
    /* Otherwise, increase congestion window by MSS for each round trip time
     * using percentage of current congestion window */
    else
    {
        tcbptr->sndcwn +=
            (tcbptr->sndmss * tcbptr->sndmss) / tcbptr->sndcwn;
    }
    return OK;
}
