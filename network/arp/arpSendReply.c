/**
 * @file arpSendReply.c
 * @provides arpSendReply
 *
 * $Id: arpSendReply.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <ethernet.h>
#include <network.h>

/**
 * Sends an ARP reply for an ARP request over a network interface. 
 * @param pkt ARP packet to reply to
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall arpSendReply(struct packet *pkt)
{
    struct netif *netptr = NULL;
    struct arpPkt *arp = NULL;
    struct netaddr dst;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    ARP_TRACE("Sending ARP reply");

    /* Setup pointers */
    netptr = pkt->nif;
    arp = (struct arpPkt *)pkt->curr;

    /* Fill in ARP header */
    arp->op = hs2net(ARP_OP_REPLY);
    ARP_TRACE("Filled in op");
    memcpy(&arp->addrs[ARP_ADDR_DHA(arp)], &arp->addrs[ARP_ADDR_SHA(arp)],
           arp->hwalen);
    memcpy(&arp->addrs[ARP_ADDR_DPA(arp)], &arp->addrs[ARP_ADDR_SPA(arp)],
           arp->pralen);
    memcpy(&arp->addrs[ARP_ADDR_SHA(arp)], netptr->hwaddr.addr,
           arp->hwalen);
    memcpy(&arp->addrs[ARP_ADDR_SPA(arp)], netptr->ip.addr, arp->pralen);
    ARP_TRACE("Filled in addrs");

    dst.type = net2hs(arp->hwtype);
    dst.len = arp->hwalen;
    memcpy(dst.addr, &arp->addrs[ARP_ADDR_DHA(arp)], dst.len);

    /* Send packet */
    return netSend(pkt, &dst, NULL, ETHER_TYPE_ARP);
}
