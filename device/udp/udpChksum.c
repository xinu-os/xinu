/**
 * @file     udpChksum.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <ipv4.h>
#include <network.h>
#include <stddef.h>
#include <string.h>
#include <udp.h>

/**
 * @ingroup udpinternal
 *
 * Calculate the checksum of a UDP packet based on UDP and IP information
 * @param udppkt UDP packet to calculate checksum for
 * @param len Length of UDP packet
 * @param src Source IP Address
 * @param dst Destination IP Address
 * @return The checksum of the UDP packet
 */
ushort udpChksum(struct packet *pkt, ushort len, const struct netaddr *src,
                 const struct netaddr *dst)
{

    struct udpPseudoHdr *pseu;
    struct udpPseudoHdr temp;
    ushort sum;

    pseu = ((struct udpPseudoHdr *)(pkt->curr)) - 1;
    memcpy(&temp, pseu, sizeof(struct udpPseudoHdr));

    /* Generate UDP pseudo header */
    memcpy(pseu->srcIp, src->addr, IPv4_ADDR_LEN);
    memcpy(pseu->dstIp, dst->addr, IPv4_ADDR_LEN);
    pseu->zero = 0;
    pseu->proto = IPv4_PROTO_UDP;
    pseu->len = hs2net(len);

    sum = netChksum(pseu, len + sizeof(struct udpPseudoHdr));

    memcpy(pseu, &temp, sizeof(struct udpPseudoHdr));

    return sum;
}
