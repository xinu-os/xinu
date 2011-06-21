/**
 * @file tcpRecvData.c
 * @provides tcpRecvData
 *
 * $Id: tcpRecvData.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * Processes the data in an incoming packet for a TCP connection.
 * Function based on RFC 763, pg 73-76.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpRecvData(struct packet *pkt, struct tcb *tcbptr)
{
    struct tcpPkt *tcp;
    ushort tcplen;
    ushort seglen;

    uint start;
    uint i;
    uint index;
    tcpseq offset;
    uchar *data;
    uint window;

    /* Setup packet pointers */
    tcp = (struct tcpPkt *)pkt->curr;
    tcplen = pkt->len - (pkt->curr - pkt->linkhdr);

    seglen = tcpSeglen(tcp, tcplen);

    /* TODO: Process URG bit */

    /* Process data */
    if (seglen > 0)
    {
        switch (tcbptr->state)
        {
        case TCP_ESTAB:
        case TCP_FINWT1:
        case TCP_FINWT2:
            /* Initialize pointer to data within TCP packet */
            data = (uchar *)tcp + offset2octets(tcp->offset);

            /* Calculate where to start in segment and buffer */
            if (seqlt(tcp->seqnum, tcbptr->rcvnxt))
            {
                offset = tcpSeqdiff(tcbptr->rcvnxt, tcp->seqnum);
                data += offset;
                seglen -= offset;
                start = tcbptr->inxt;
            }
            else
            {
                offset = tcpSeqdiff(tcp->seqnum, tcbptr->rcvnxt);
                start = (tcbptr->inxt + offset) % TCP_IBLEN;
            }

            /* Copy only part of data if not enough buffer space */
            window = tcpSeqdiff(tcbptr->rcvwnd, tcbptr->rcvnxt);
            window -= offset;
            if (seglen > window)
            {
                seglen = window;
                tcp->control &= ~TCP_CTRL_FIN;
            }

            /* Copy data into buffer */
            for (i = 0; i < seglen; i++)
            {
                index = (start + i) % TCP_IBLEN;
                tcbptr->in[index] = data[i];
                tcbptr->imark[index] = TRUE;
            }

            /* If started at begnning of window, we can ACK */
            if (start == tcbptr->inxt)
            {
                /* ACK at least current data */
                tcbptr->icount += seglen;
                tcbptr->ibytes += seglen;
                tcbptr->inxt = (tcbptr->inxt + seglen) % TCP_IBLEN;
                tcbptr->rcvnxt = seqadd(tcbptr->rcvnxt, seglen);

                /* Keep going until data is missing */
                while ((tcbptr->icount < TCP_IBLEN)
                       && (tcbptr->imark[tcbptr->inxt]))
                {
                    tcbptr->icount++;
                    tcbptr->ibytes++;
                    tcbptr->inxt = (tcbptr->inxt + 1) % TCP_IBLEN;
                    tcbptr->rcvnxt = seqadd(tcbptr->rcvnxt, 1);

                    /* If FIN has been seen, stop when we reach it */
                    if ((tcbptr->rcvflg & TCP_FLG_FIN)
                        && (tcbptr->rcvnxt == tcbptr->rcvfin))
                    {
                        tcp->control |= TCP_CTRL_FIN;
                        break;
                    }
                }

                /* Signal readers */
                if (semcount(tcbptr->readers) < 1)
                {
                    signal(tcbptr->readers);
                }

                tcbptr->sndflg |= TCP_FLG_SNDACK;
            }

            break;

            /* Data should not be recevied in CLOSEWT, CLOSING, LASTACK, and TIMEWT
             * states since a FIN has already been received from remote side;
             * ignore data. */
        }
    }

    /* Process FIN bit */
    if (tcp->control & TCP_CTRL_FIN)
    {
        /* If FIN was not received previously, store FIN sequence number */
        if (!(tcbptr->rcvflg & TCP_FLG_FIN))
        {
            tcbptr->rcvfin = seqadd(tcp->seqnum, seglen);
            tcbptr->rcvflg |= TCP_FLG_FIN;
            TCP_TRACE("Store FIN");
        }

        /* If FIN is expected next, process the FIN now */
        if (tcbptr->rcvnxt == tcbptr->rcvfin)
        {
            TCP_TRACE("Process FIN, state %d", tcbptr->state);
            if (semcount(tcbptr->readers) < 0)
            {
                signaln(tcbptr->readers, semcount(tcbptr->readers) * -1);
            }

            switch (tcbptr->state)
            {
            case TCP_SYNRECV:
            case TCP_ESTAB:
                tcbptr->rcvnxt = seqadd(tcbptr->rcvnxt, 1);
                tcbptr->state = TCP_CLOSEWT;
                break;
            case TCP_FINWT1:
                tcbptr->rcvnxt = seqadd(tcbptr->rcvnxt, 1);
                if (seqlt(tcbptr->sndfin, tcbptr->snduna))
                {
                    tcpTimerSched(TCP_TWOMSL, tcbptr, TCP_EVT_TIMEWT);
                    tcpTimerPurge(tcbptr, NULL);
                    tcbptr->state = TCP_TIMEWT;
                }
                else
                {
                    tcbptr->state = TCP_CLOSING;
                }
                break;
            case TCP_FINWT2:
                tcbptr->rcvnxt = seqadd(tcbptr->rcvnxt, 1);
                tcpTimerPurge(tcbptr, NULL);
                tcpTimerSched(TCP_TWOMSL, tcbptr, TCP_EVT_TIMEWT);
                break;
                /* If in TIMEWT, CLOSEWT, CLOSING, or LASTACK states, then 
                 * FIN has already been received; send an ACK */
            case TCP_TIMEWT:
                tcpTimerPurge(tcbptr, NULL);
                tcpTimerSched(TCP_TWOMSL, tcbptr, TCP_EVT_TIMEWT);
            case TCP_CLOSEWT:
            case TCP_CLOSING:
            case TCP_LASTACK:
                tcbptr->sndflg |= TCP_FLG_SNDACK;
                break;
            }

            tcbptr->sndflg |= TCP_FLG_SNDACK;
        }
    }

    /* Attempt to send data */
    if (tcbptr->sndflg & TCP_FLG_SNDDATA)
    {
        tcpSendData(tcbptr);
    }

    /* Ack if needed */
    if (tcbptr->sndflg & TCP_FLG_SNDACK)
    {
        tcpSendAck(tcbptr);
    }

    return OK;
}
