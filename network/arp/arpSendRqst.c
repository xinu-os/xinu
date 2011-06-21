/**
 * @file arpSendRqst.c
 * @provides arpSendRqst
 *
 * $Id: arpSendRqst.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <ethernet.h>
#include <network.h>

/**
 * Sends an ARP request for an ARP table entry over a network interface. 
 * @param entry ARP table entry
 * @return OK if packet was sent, otherwise SYSERR
 */
syscall arpSendRqst(struct arpEntry *entry)
{
    struct netif *netptr = NULL;
    struct packet *pkt = NULL;
    struct arpPkt *arp = NULL;
    int result;

    /* Error check pointers */
    if (NULL == entry)
    {
        return SYSERR;
    }

    ARP_TRACE("Sending ARP request");

    /* Setup pointer to network interface */
    netptr = entry->nif;

    /* Obtain a buffer for the packet */
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        ARP_TRACE("Failed to acquire packet buffer");
        return SYSERR;
    }

    /* Place ARP header at end of packet buffer */
    pkt->nif = netptr;
    pkt->len =
        ARP_CONST_HDR_LEN + netptr->hwaddr.len * 2 + netptr->ip.len * 2;
    pkt->curr -= pkt->len;
    arp = (struct arpPkt *)pkt->curr;

    /* Fill in ARP header */
    arp->hwtype = hs2net(netptr->hwaddr.type);
    arp->prtype = hs2net(netptr->ip.type);
    arp->hwalen = netptr->hwaddr.len;
    arp->pralen = netptr->ip.len;
    arp->op = hs2net(ARP_OP_RQST);
    ARP_TRACE("Filled in types, lens, and op");
    memcpy(&arp->addrs[ARP_ADDR_SHA(arp)], netptr->hwaddr.addr,
           arp->hwalen);
    memcpy(&arp->addrs[ARP_ADDR_SPA(arp)], netptr->ip.addr, arp->pralen);
    memcpy(&arp->addrs[ARP_ADDR_DPA(arp)], entry->praddr.addr,
           arp->pralen);
    ARP_TRACE("Filled in addrs");

    /* Send packet */
    result = netSend(pkt, &netptr->hwbrc, NULL, ETHER_TYPE_ARP);

    ARP_TRACE("Sent packet");

    /* Free buffer for the packet */
    if (SYSERR == netFreebuf(pkt))
    {
        ARP_TRACE("Failed to free packet buffer");
        return SYSERR;
    }

    /* Return result of netSend */
    return result;
}
