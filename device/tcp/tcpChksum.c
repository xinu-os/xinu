/**
 * @file tcpChksum.c
 * @provides tcpChcksum
 *
 * $Id: tcpChksum.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>
#include <tcp.h>

ushort tcpChksum(struct packet *pkt, ushort len, struct netaddr *src,
                 struct netaddr *dst)
{
    struct tcpPseudo *pseu;
    uchar buf[TCP_PSEUDO_LEN];
    ushort sum;

    /* Store current data before TCP header in temporary buffer */
    pseu = (struct tcpPseudo *)(pkt->curr - TCP_PSEUDO_LEN);
    memcpy(buf, pseu, TCP_PSEUDO_LEN);

    /* Generate TCP psuedo header */
    memcpy(pseu->srcIp, src->addr, IPv4_ADDR_LEN);
    memcpy(pseu->dstIp, dst->addr, IPv4_ADDR_LEN);
    pseu->zero = 0;
    pseu->proto = IPv4_PROTO_TCP;
    pseu->len = hs2net(len);

    sum = netChksum(pseu, len + TCP_PSEUDO_LEN);

    /* Restore data before TCP header from temporary buffer */
    memcpy(pseu, buf, TCP_PSEUDO_LEN);

    return sum;
}
