/**
 * file ipv4Send.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>
#include <string.h>
#include <route.h>

/**
 * @ingroup ipv4
 *
 * Send an outgoing IPv4 packet.
 * @param packet packet being sent
 * @param src source IP address
 * @param dst destination IP address
 * @param proto the protocol of the ip pkt
 * @return OK if packet was sent, TIMEOUT if ARP request timed out,
 * IPv4_NO_INTERFACE if interface does not exist, IPv4_NO_HOP if next hop
 * is unknown, SYSERR otherwise.
 */
syscall ipv4Send(struct packet *pkt, struct netaddr *src,
                 struct netaddr *dst, uchar proto)
{
    struct rtEntry *rtptr;
    struct ipv4Pkt *ip;
    struct netaddr *nxthop;

    /* Error check pointers */
    if ((NULL == pkt) || (NULL == dst))
    {
        IPv4_TRACE("Invalid args");
        return SYSERR;
    }
    if (dst->type != NETADDR_IPv4)
    {
        IPv4_TRACE("Invalid dst type");
        return SYSERR;
    }

    /* Lookup destination in route table */
    rtptr = rtLookup(dst);
    if (NULL == rtptr)
    {
        IPv4_TRACE("No route");
        return SYSERR;
    }

    /* Packet has next hop in route table */
    pkt->nif = rtptr->nif;
    if (NULL == rtptr->gateway.type)
    {
        IPv4_TRACE("Next hop is dst");
        nxthop = dst;
    }
    else
    {
        IPv4_TRACE("Next hop is gateway");
        nxthop = &rtptr->gateway;
    }

    /* Set up outgoing packet header */
    pkt->len += IPv4_HDR_LEN;
    pkt->curr -= IPv4_HDR_LEN;

    ip = (struct ipv4Pkt *)pkt->curr;

    /* Set up the IP packet header */
    ip->ver_ihl = (uchar)(IPv4_VERSION << 4);
    ip->ver_ihl += IPv4_HDR_LEN / 4;
    ip->tos = IPv4_TOS_ROUTINE;
    ip->len = hs2net(pkt->len);
    ip->id = 0;
    ip->flags_froff = 0;
    ip->ttl = IPv4_TTL;
    ip->proto = proto;
    if (NULL == src->type)
    {
        /* No source was specified, use IP of outgoing network interface */
        memcpy(ip->src, pkt->nif->ip.addr, IPv4_ADDR_LEN);
    }
    else
    {
        /* Use provided source IP */
        memcpy(ip->src, src->addr, IPv4_ADDR_LEN);
    }
    memcpy(ip->dst, dst->addr, IPv4_ADDR_LEN);

    /* Calculate checksum */
    ip->chksum = 0;
    ip->chksum = netChksum((uchar *)ip, IPv4_HDR_LEN);
    IPv4_TRACE("Setup IPv4 header");

    /* Fragment and send packet */
    return ipv4SendFrag(pkt, nxthop);
}
