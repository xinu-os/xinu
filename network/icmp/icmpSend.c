/**
 * @file icmpSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <icmp.h>
#include <ethernet.h>
#include <network.h>

/**
 * @ingroup icmp
 *
 * Sends an ICMP reply.
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall icmpSend(struct packet *pkt, uchar type, uchar code,
                 int datalen, struct netaddr *dst)
{
    struct icmpPkt *icmp = NULL;
    struct netaddr src;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    pkt->curr -= ICMP_HEADER_LEN;
    pkt->len += ICMP_HEADER_LEN;
    icmp = (struct icmpPkt *)pkt->curr;

    icmp->type = type;
    icmp->code = code;
    icmp->chksum = 0;
    icmp->chksum = netChksum((uchar *)icmp, datalen + ICMP_HEADER_LEN);

    src.type = NULL;

    ICMP_TRACE("Sending ICMP packet type %d, code %d", type, code);
    return ipv4Send(pkt, &src, dst, IPv4_PROTO_ICMP);
}
