/**
 * @file icmpEchoReply.c
 * @provides icmpEchoReply
 *
 * $Id: icmpEchoReply.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <icmp.h>
#include <ipv4.h>
#include <stdlib.h>

/**
 * Compose ICMP Echo Reply.
 * @param pkt Packet containing request
 * @param nif Net interface matching destination address
 * @return OK on success, else SYSERR
 */
syscall icmpEchoReply(struct packet *pkt, struct netif *nif)
{
    struct icmpPkt *icmp = NULL;
    struct ipv4Pkt *ip = NULL;
    struct netaddr dst;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    ip = (struct ipv4Pkt *)pkt->nethdr;
    icmp = (struct icmpPkt *)pkt->curr;

    icmp->type = hs2net(ICMP_ECHOREPLY);
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
    memcpy(dst.addr, ip->src, dst.len);

    ICMP_TRACE("Sending Echo Reply");
    ipv4Send(pkt, &nif->ip, &dst, IPv4_PROTO_ICMP);

    return OK;
}
