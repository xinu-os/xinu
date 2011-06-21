/**
 * @file icmpRedirect.c
 * @provides icmpRedirect
 *
 * $Id: icmpRedirect.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <icmp.h>
#include <stdlib.h>
#include <route.h>

/**
 * Compose ICMP Redirect message.
 * @param redir packet that should have gone somewhere else
 * @param code  ICMP redirect code number
 * @param route new gateway for packet
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall icmpRedirect(struct packet *redir, int code,
                     struct rtEntry *route)
{
    struct packet *pkt = NULL;
    struct ipv4Pkt *ip = NULL;
    struct netaddr dst;
    int result = OK;
    int ihl = 0;

    ICMP_TRACE("ICMP redirect, code(%d)", code);
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ICMP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    ip = (struct ipv4Pkt *)redir->nethdr;
    dst.type = NETADDR_IPv4;
    dst.len = IPv4_ADDR_LEN;
    /* Send error message back to original source.                */
    memcpy(dst.addr, ip->src, dst.len);
    ihl = (ip->ver_ihl & IPv4_IHL) * 4;

    /* Message will contain at least ICMP_DEF_DATALEN             */
    /*  of packet in question, as per RFC 792.                    */
    pkt->len = ihl + ICMP_DEF_DATALEN;
    pkt->curr -= pkt->len;

    memcpy(pkt->curr, ip, ihl + ICMP_DEF_DATALEN);
    /* First four octets of payload are gateway.                  */
    pkt->curr -= IPv4_ADDR_LEN;
    pkt->len += IPv4_ADDR_LEN;

    memcpy(pkt->curr, route->gateway.addr, IPv4_ADDR_LEN);

    result = icmpSend(pkt, ICMP_REDIRECT, code, pkt->len, &dst);

    netFreebuf(pkt);
    return result;
}
