/**
 * @file tcpRecvValid.c
 * @provides tcpRecvValid
 *
 * $Id: tcpRecvValid.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * Checks if an incoming TCP segment is valid.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @precondition TCB mutex is already held 
 * @postcondition TCB mutex is still held
 */
bool tcpRecvValid(struct packet *pkt, struct tcb *tcbptr)
{
    struct tcpPkt *tcp;
    ushort tcplen;
    tcpseq temp;
    bool result;
    bool resultB;
    ushort seglen;
    uint availwnd;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;
    tcplen = pkt->len - (pkt->curr - pkt->linkhdr);

    seglen = tcpSeglen(tcp, tcplen);
    availwnd = tcpSeqdiff(tcbptr->rcvwnd, tcbptr->rcvnxt);

    /* Add SYN and FIN */
    if (tcp->control & TCP_CTRL_SYN)
    {
        seglen++;
    }
    if (tcp->control & TCP_CTRL_FIN)
    {
        seglen++;
    }

    /* SEGLEN = 0 , WND = 0
     * SEGSEQ = RCVNXT */
    if ((0 == seglen) && (0 == availwnd))
    {
        result = (tcp->seqnum == tcbptr->rcvnxt);
    }

    /* SEGLEN = 0, WND > 0
     * RCVNEXT =< SEGSEQ < RCVWND */
    if ((0 == seglen) && (availwnd > 0))
    {
        result = seqlte(tcbptr->rcvnxt, tcp->seqnum);
        result &= seqlt(tcp->seqnum, tcbptr->rcvwnd);
    }

    /* SEGLEN > 0, WND = 0
     * Not acceptable */
    if ((seglen > 0) && (0 == availwnd))
    {
        result = FALSE;
    }

    /* SEGLEN > 0, WND > 0
     * RCVNXT =< SEGSEQ < RCVWND OR
     * RCVNXT =< SEGSEQ + SEGLEN - 1 < RCVWND */
    if ((seglen > 0) && (availwnd > 0))
    {
        result = seqlte(tcbptr->rcvnxt, tcp->seqnum);
        result &= seqlt(tcp->seqnum, tcbptr->rcvwnd);

        temp = seqadd(tcp->seqnum, seglen - 1);
        resultB = seqlte(tcbptr->rcvnxt, temp);
        resultB &= seqlt(temp, tcbptr->rcvwnd);

        result = (result || resultB);
    }

    /* If WND = 0, only process ACK, URG, and RST */
    if (availwnd == 0)
    {
        tcp->control &= ~(TCP_CTRL_PSH | TCP_CTRL_SYN | TCP_CTRL_FIN);
        pkt->len -= seglen;
    }

    return result;
}
