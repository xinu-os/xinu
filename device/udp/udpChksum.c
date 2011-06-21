/**
 * @file     udpChksum.c
 * @provides udpChksum
 *
 * $Id: udpChksum.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <ipv4.h>
#include <network.h>
#include <stddef.h>
#include <string.h>
#include <udp.h>

/**
 * Calculate the checksum of a UDP packet based on UDP and IP information
 * @param udppkt UDP packet to calculate checksum for
 * @param len Length of UDP packet
 * @param src Source IP Address
 * @param dst Destination IP Address
 * @return The checksum of the UDP packet
 */
ushort udpChksum(struct packet *pkt, ushort len, struct netaddr *src,
                 struct netaddr *dst)
{

    struct udpPseudoHdr *pseu;
    uchar buf[UDP_PSEUDO_LEN];
    ushort sum;

    pseu = (struct udpPseudoHdr *)(pkt->curr - UDP_PSEUDO_LEN);
    memcpy(buf, pseu, UDP_PSEUDO_LEN);

    /* Generate UDP pseudo header */
    memcpy(pseu->srcIp, src->addr, IPv4_ADDR_LEN);
    memcpy(pseu->dstIp, dst->addr, IPv4_ADDR_LEN);
    pseu->zero = 0;
    pseu->proto = IPv4_PROTO_UDP;
    pseu->len = hs2net(len);

    sum = netChksum(pseu, len + UDP_PSEUDO_LEN);

    memcpy(pseu, buf, UDP_PSEUDO_LEN);

    return sum;
}
