/**
 * @file icmpTimeExceeded.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <icmp.h>
#include <stdlib.h>

/**
 * @ingroup icmp
 *
 * Compose ICMP Time Exceeded message.
 * @param unreached packet that could not be sent
 * @param code      ICMP destination unreachable code number
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall icmpTimeExceeded(struct packet *unreached, int code)
{
    struct packet *pkt = NULL;
    struct ipv4Pkt *ip = NULL;
    struct netaddr dst;
    int result = OK;
    int ihl = 0;

    ICMP_TRACE("Time exceeded (%d)", code);
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ICMP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    ip = (struct ipv4Pkt *)unreached->nethdr;
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
    /* First four octets of payload are unused.                   */
    pkt->curr -= 4;
    pkt->len += 4;
    *((ulong *)pkt->curr) = 0;

    result = icmpSend(pkt, ICMP_TIMEEXCD, code, pkt->len, &dst);

    netFreebuf(pkt);
    return result;
}
