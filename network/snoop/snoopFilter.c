/* @file snoopFilter.c
 * @provides snoopFilter
 *
 * $Id: snoopFilter.c 2074 2009-09-21 23:37:28Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <ipv4.h>
#include <network.h>
#include <snoop.h>
#include <string.h>

/**
 * Determine if a packet matches the filter.
 * @return TRUE if packet matches filter, otherwise FALSE
 */
bool snoopFilter(struct snoop *s, struct packet *pkt)
{
    struct etherPkt *ether = NULL;
    struct arpPkt *arp = NULL;
    struct ipv4Pkt *ip = NULL;

    /* Packet matches filter if there is no filter */
    if ((NULL == s->type)
        && (NULL == s->srcaddr.type) && (NULL == s->srcport)
        && (NULL == s->dstaddr.type) && (NULL == s->dstport))
    {
        return TRUE;
    }

    /* Ensure packet is valid size, otherwise it can't be filtered */
    if (pkt->len < ETH_HDR_LEN)
    {
        /* Packet is too small to have Ethernet header */
        return FALSE;
    }
    else
    {
        ether = (struct etherPkt *)pkt->curr;
        switch (net2hs(ether->type))
        {
        case ETHER_TYPE_ARP:
            if (pkt->len < (ETH_HDR_LEN + ARP_CONST_HDR_LEN))
            {
                /* Packet is too small to have ARP header */
                return FALSE;
            }
            arp = (struct arpPkt *)ether->data;
            if (pkt->len < (ETH_HDR_LEN + ARP_CONST_HDR_LEN +
                            (2 * arp->hwalen) + (2 * arp->pralen)))
            {
                /* Packet is too small to have ARP header */
                return FALSE;
            }
            break;
        case ETHER_TYPE_IPv4:
            if (pkt->len < (ETH_HDR_LEN + IPv4_HDR_LEN))
            {
                /* Packet is too small to have IPv4 header */
                return FALSE;
            }
            ip = (struct ipv4Pkt *)ether->data;
            break;
        default:
            /* Don't know how to filter other Ethernet types */
            return FALSE;
        }
    }

    /* If code reaches here, the packet is a valid length */

    /* Check if packet type matches */
    if (s->type != NULL)
    {
        /* Check Ethernet type */
        switch (net2hs(ether->type))
        {
        case ETHER_TYPE_ARP:
            if (s->type != SNOOP_FILTER_ARP)
            {
                return FALSE;
            }
            break;
        case ETHER_TYPE_IPv4:
            if (s->type != SNOOP_FILTER_IPv4)
            {
                /* Check IP protocol */
                switch (ip->proto)
                {
                case IPv4_PROTO_UDP:
                    if (s->type != SNOOP_FILTER_UDP)
                    {
                        return FALSE;
                    }
                    break;
                case IPv4_PROTO_TCP:
                    if (s->type != SNOOP_FILTER_TCP)
                    {
                        return FALSE;
                    }
                    break;
                case IPv4_PROTO_ICMP:
                    if (s->type != SNOOP_FILTER_ICMP)
                    {
                        return FALSE;
                    }
                    break;
                default:
                    return FALSE;
                }
            }
            break;
        default:
            return FALSE;
        }
    }

    /* If code reaches here, either there is no type filter 
     * or the packet matches the type filter */

    /* Check if packet source address matches */
    if (s->srcaddr.type != NULL)
    {
        /* ARP */
        if (arp != NULL)
        {
            if ((s->srcaddr.len != arp->pralen)
                || (0 != memcmp(&arp->addrs[ARP_ADDR_SPA(arp)],
                                s->srcaddr.addr, arp->pralen)))
            {
                return FALSE;
            }
        }
        /* IPv4 */
        else if (ip != NULL)
        {
            if ((s->srcaddr.len != IPv4_ADDR_LEN)
                || (0 != memcmp(ip->src, s->srcaddr.addr, IPv4_ADDR_LEN)))
            {
                return FALSE;
            }
        }
        /* Unknown */
        else
        {
            return FALSE;
        }
    }

    /* Check if packet destination address matches */
    if (s->dstaddr.type != NULL)
    {
        /* ARP */
        if (arp != NULL)
        {
            if ((s->dstaddr.len != arp->pralen)
                || (0 != memcmp(&arp->addrs[ARP_ADDR_DPA(arp)],
                                s->dstaddr.addr, arp->pralen)))
            {
                return FALSE;
            }
        }
        /* IPv4 */
        else if (ip != NULL)
        {
            if ((s->dstaddr.len != IPv4_ADDR_LEN)
                || (0 != memcmp(ip->dst, s->dstaddr.addr, IPv4_ADDR_LEN)))
            {
                return FALSE;
            }
        }
        /* Unknown */
        else
        {
            return FALSE;
        }
    }

    /* Check if packet ports match */
    if ((s->srcport != NULL) || (s->dstport != NULL))
    {
        if (ip == NULL)
        {
            /* Only IP packets can be filtered by port */
            return FALSE;
        }

        switch (ip->proto)
        {
            //case IPv4_PROTO_TCP:
            /* TODO */
            //      break;
            //case IPv4_PROTO_UDP:
            /* TODO */
            //      break;
            //case IPv4_PROTO_ICMP:
            /* TODO */
            //      break;
        default:
            return FALSE;
        }
    }

    return TRUE;
}
