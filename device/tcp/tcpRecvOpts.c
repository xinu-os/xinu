/**
 * @file tcpRecvOpts.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Processes the options in an incoming packet for a TCP connection.
 * @param pkt incoming packet
 * @param tcbptr pointer to transmission control block for connection
 * @return OK
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpRecvOpts(struct packet *pkt, struct tcb *tcbptr)
{
    uchar *options;
    uchar *endopt;
    struct tcpPkt *tcp;

    tcp = (struct tcpPkt *)pkt->curr;

    /* Check if the header contains options */
    if (offset2octets(tcp->offset) == TCP_HDR_LEN)
    {
        /* No options */
        return OK;
    }

    options = tcp->data;
    endopt = options + (offset2octets(tcp->offset) - TCP_HDR_LEN);

    /* Keep handling options until end of otpion list is encountered */
    while ((*options != TCP_OPT_END) && (options < endopt))
    {
        switch (*options)
        {
            /* Maximum segment size */
        case TCP_OPT_MSS:
            options += 2;
            /*
             * BUG: The options may not be word aligned to deference as
             * a short.  Discovered by RB on 7/2.
             * FIXED by AG on 8/10.
             * TODO: add test case with non-word aligned opts
             */
            tcbptr->sndmss = *options++;
            tcbptr->sndmss += *options++ << 8;
            tcbptr->sndmss -= TCP_HDR_LEN;
            break;
            /* Skip over NOP and unknown options */
        case TCP_OPT_NOP:
        default:
            options++;
            break;
        }
    }

    return OK;
}
