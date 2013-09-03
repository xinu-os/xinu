/**
 * file netSend.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <device.h>
#include <ethernet.h>
#include <network.h>
#include <snoop.h>
#include <string.h>

/**
 * @ingroup network
 *
 * Appends the Link-Level header to a packet and writes to the 
 * underlying interface.
 * @param pkt packet to send
 * @param hwaddr hardware address of the destination, NULL if should lookup
 * @param praddr protocol address of the destination, NULL if hwaddr is known
 * @param type type of the packet to put in link level header
 * @return OK if packet was sent, TIMEOUT if ARP request timed out,
 * 	otherwise SYSERR
 */
syscall netSend(struct packet *pkt, const struct netaddr *hwaddr,
                const struct netaddr *praddr, ushort type)
{
    struct netif *netptr = NULL;        /**< pointer to network interface */
    struct etherPkt *ether = NULL;      /**< pointer to Ethernet header   */
    int result;                         /**< result of ARP lookup         */
    struct netaddr addr;

    /* Setup and error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }
    netptr = pkt->nif;
    if ((NULL == netptr) || (netptr->state != NET_ALLOC))
    {
        return SYSERR;
    }

    NET_TRACE("Send packet of type 0x%04X", type);

    /* Make space for Link-Level header */
    pkt->curr -= netptr->linkhdrlen;
    pkt->len += netptr->linkhdrlen;
    ether = (struct etherPkt *)(pkt->curr);

    /* Setup Ethernet header */
    ether->type = hs2net(type);
    memcpy(ether->src, netptr->hwaddr.addr, netptr->hwaddr.len);
#ifdef TRACE_NET
    char str[20];
    netaddrsprintf(str, &netptr->hwaddr);
    NET_TRACE("Src = %s", str);
#endif

    /* If no hardware address was specified, lookup using protocol address */
    if (NULL == hwaddr)
    {
        NET_TRACE("Hardware address lookup required");
        hwaddr = &addr;
        result = arpLookup(netptr, praddr, (struct netaddr*)hwaddr);
        if (result != OK)
        {
            return result;
        }
    }

    /* Copy destination hardware address into link-level header */
    memcpy(ether->dst, hwaddr->addr, hwaddr->len);

    /* Write the packet to the underlying device */
    if (pkt->len != write(netptr->dev, pkt->curr, pkt->len))
    {
        return SYSERR;
    }

    /* Snoop packet */
    if (netptr->capture != NULL)
    {
        snoopCapture(netptr->capture, pkt);
    }

    return OK;
}
