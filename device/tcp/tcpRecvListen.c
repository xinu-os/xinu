/**
 * @file tcpRecvListen.c
 * @provides tcpRecvListen
 *
 * $Id: tcpRecvListen.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * Processes an incoming packet for a TCP connection in the LISTEN state.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @param src source IP address
 * @return OK 
 * @pre-condition TCB mutex is already held
 */
int tcpRecvListen(struct packet *pkt, struct tcb *tcbptr,
                  struct netaddr *src)
{
    struct tcpPkt *tcp;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;

    /* Ignore reset */
    if (tcp->control & TCP_CTRL_RST)
    {
        return OK;
    }

    /* Should not receive ACK, send reset if received */
    if (tcp->control & TCP_CTRL_ACK)
    {
        tcbptr->sndflg |= TCP_FLG_SNDRST;
        return OK;
    }

    /* Should receive a SYN */
    if (tcp->control & TCP_CTRL_SYN)
    {
        /* Update receive information */
        tcbptr->rcvnxt = seqadd(tcp->seqnum, 1);
        tcbptr->rcvwnd = seqadd(tcp->seqnum, TCP_INIT_WND);
        tcbptr->rcvflg |= TCP_FLG_SYN;
        tcbptr->sndflg |= TCP_FLG_SNDACK;

        /* Finish specifying connection, if not already set */
        if (NULL == tcbptr->remotept)
        {
            tcbptr->remotept = tcp->srcpt;
        }
        if (NULL == tcbptr->remoteip.type)
        {
            netaddrcpy(&tcbptr->remoteip, src);
        }

        /* Update send information */
        tcbptr->sndwnd = tcp->window;
        tcbptr->sndwl1 = tcp->seqnum;

        /* Attempt to send SYN */
        tcpSendSyn(tcbptr);

        /* Ack if needed */
        if (tcbptr->sndflg & TCP_FLG_SNDACK)
        {
            tcpSendAck(tcbptr);
        }

        /* Change state */
        tcbptr->state = TCP_SYNRECV;

        /* Processing remaining controls and data */
        return tcpRecvData(pkt, tcbptr);
    }

    /* Should not get here, but if so drop segment and return */
    return OK;
}
