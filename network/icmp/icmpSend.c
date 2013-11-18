/**
 * @file icmpSend.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <ipv4.h>
#include <icmp.h>
#include <network.h>

/**
 * @ingroup icmp
 *
 * Send an ICMP (Internet Control Message Protocol) packet.
 *
 * @param pkt
 *      Packet containing the ICMP data.  pkt->curr must be positioned on the
 *      ICMP data, pkt->len must be the length of the ICMP data, and there must
 *      be at least enough space between pkt->data and pkt->curr for the ICMP,
 *      IPv4, and link-level headers to be attached.
 * @param type
 *      ICMP type field to use.
 * @param code
 *      ICMP code field to use.
 * @param datalen
 *      Length of the ICMP data payload.
 * @param src
 *      Source network address.  If src->type == 0, then the source is
 *      automatically set to the address of the interface on which the packet is
 *      sent.
 * @paramr dst
 *      Destination network address.
 *
 * @return
 *      ::OK if packet was successfully sent; otherwise ::SYSERR or another
 *      error code returned by ipv4Send().
 */
syscall icmpSend(struct packet *pkt, uchar type, uchar code,
                 uint datalen, struct netaddr *src, struct netaddr *dst)
{
    struct icmpPkt *icmp;

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

    ICMP_TRACE("Sending ICMP packet type %d, code %d", type, code);
    return ipv4Send(pkt, src, dst, IPv4_PROTO_ICMP);
}
