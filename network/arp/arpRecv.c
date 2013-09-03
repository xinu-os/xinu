/**
 * @file arpRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <clock.h>
#include <ethernet.h>
#include <interrupt.h>
#include <ipv4.h>
#include <mailbox.h>
#include <network.h>
#include <string.h>

/**
 * @ingroup arp
 *
 * Processes an incoming ARP packet.
 * @param pkt pointer to the incoming packet
 * return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall arpRecv(struct packet *pkt)
{
    struct netif *netptr = NULL;
    struct arpPkt *arp = NULL;
    struct arpEntry *entry = NULL;  /**< pointer to ARP table entry     */
    struct netaddr sha;             /**< source hardware address        */
    struct netaddr spa;             /**< source protocol address        */
    struct netaddr dpa;             /**< destination protocol address   */
    irqmask im;                     /**< interrupt state                */

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    /* Setup pointers to ARP header and network interface */
    pkt->nethdr = pkt->curr;
    arp = (struct arpPkt *)pkt->curr;
    netptr = pkt->nif;
    if (NULL == netptr || NULL == arp)
    {
        netFreebuf(pkt);
        return SYSERR;
    }

    ARP_TRACE("Processing ARP packet");

    /* Verify hardware type is Ethernet and address length is correct */
    if ((ARP_HWTYPE_ETHERNET != net2hs(arp->hwtype))
        || (ETH_ADDR_LEN != arp->hwalen))
    {
        ARP_TRACE("Hardware type not Ethernet");
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Verify protocol type is IPv4 and address length is correct */
    if ((ETHER_TYPE_IPv4 != net2hs(arp->prtype))
        || (IPv4_ADDR_LEN != arp->pralen))
    {
        ARP_TRACE("Protocol type not IPv4");
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Obtain source hardware address */
    sha.type = net2hs(arp->hwtype);
    sha.len = arp->hwalen;
    memcpy(sha.addr, &arp->addrs[0], sha.len);

    /* Obtain source protocol address */
    spa.type = net2hs(arp->prtype);
    spa.len = arp->pralen;
    memcpy(spa.addr, &arp->addrs[arp->hwalen], spa.len);

    /* Update existing entry if it exists */
    im = disable();
    entry = arpGetEntry(&spa);
    if (entry != NULL)
    {
        ARP_TRACE("Entry already exists");
        netaddrcpy(&entry->hwaddr, &sha);
        entry->expires = clktime + ARP_TTL_RESOLVED;

        /* Notify threads waiting on resolution */
        if (ARP_UNRESOLVED == entry->state)
        {
            entry->state = ARP_RESOLVED;
            arpNotify(entry, ARP_MSG_RESOLVED);
            ARP_TRACE("Notified waiting threads");
        }
    }

    /* Obtain destination protocol address */
    dpa.type = net2hs(arp->prtype);
    dpa.len = arp->pralen;
    memcpy(dpa.addr, &arp->addrs[arp->hwalen * 2 + arp->pralen], dpa.len);

    /* Verify protocol address is mine */
    if (netaddrequal(&netptr->ip, &dpa))
    {
        /* If entry did not already exist, then entry should be added */
        if (NULL == entry)
        {
            entry = arpAlloc();
            if (SYSERR == (int)entry)
            {
                restore(im);
                netFreebuf(pkt);
                return SYSERR;
            }

            entry->state = ARP_RESOLVED;
            entry->nif = pkt->nif;
            netaddrcpy(&entry->hwaddr, &sha);
            netaddrcpy(&entry->praddr, &spa);
            entry->expires = clktime + ARP_TTL_RESOLVED;
            ARP_TRACE("Added entry %d (state = %d)",
                      ((int)entry -
                       (int)arptab) / sizeof(struct arpEntry),
                      entry->state);
        }

        /* If entry is a request, send a reply */
        if (ARP_OP_RQST == net2hs(arp->op))
        {
            if (mailboxCount(arpqueue) >= ARP_NQUEUE)
            {
                restore(im);
                netFreebuf(pkt);
                return SYSERR;
            }
            mailboxSend(arpqueue, (int)pkt);
            ARP_TRACE("Enqueued request for daemon to reply");
            restore(im);
            return OK;
        }
    }

    restore(im);
    netFreebuf(pkt);
    return OK;
}
