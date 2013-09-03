/**
 * @file snoopPrint.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <ctype.h>
#include <ethernet.h>
#include <ipv4.h>
#include <network.h>
#include <snoop.h>
#include <stdio.h>
#include <string.h>
#include <tcp.h>
#include <udp.h>

/**
 * @ingroup snoop
 *
 * Outputs snoop information for a packet.
 * @param pkt packet
 * @param dump dump level
 * @param verbose verbosity level
 * @return OK, SYSERR if error occurs
 */
int snoopPrint(struct packet *pkt, char dump, char verbose)
{
    struct etherPkt *ether = NULL;
    struct arpPkt *arp = NULL;
    struct ipv4Pkt *ip = NULL;
    uchar *appHdr = NULL;
    struct netaddr addr;
    char strA[20];
    char strB[20];
    char strC[20];
    int i = 0;
    int j = 0;
    uchar ch;

    if (NULL == pkt)
    {
        return SYSERR;
    }

    /* TODO: Print timestamp */

    /* Print packet summary */
    ether = (struct etherPkt *)pkt->curr;
    switch (net2hs(ether->type))
    {
    case ETHER_TYPE_ARP:
        arp = (struct arpPkt *)ether->data;
        /* strA = destination protocol address */
        addr.type = net2hs(arp->prtype);
        addr.len = arp->pralen;
        memcpy(addr.addr, &arp->addrs[ARP_ADDR_DPA(arp)], addr.len);
        netaddrsprintf(strA, &addr);
        /* strB = source protocol address */
        memcpy(addr.addr, &arp->addrs[ARP_ADDR_SPA(arp)], addr.len);
        netaddrsprintf(strB, &addr);
        /* strC = source hardware address */
        addr.type = net2hs(arp->hwtype);
        addr.len = arp->hwalen;
        memcpy(addr.addr, &arp->addrs[ARP_ADDR_SHA(arp)], addr.len);
        netaddrsprintf(strC, &addr);
        /* Output summary based on operation */
        switch (net2hs(arp->op))
        {
        case ARP_OP_RQST:
            printf("arp who-has %s tell %s", strA, strB);
            break;
        case ARP_OP_REPLY:
            printf("arp reply %s is-at %s", strA, strC);
            break;
        default:
            printf("arp unknown from %s to %s", strB, strA);
            break;
        }
        break;
    case ETHER_TYPE_IPv4:
        ip = (struct ipv4Pkt *)ether->data;
        /* strA = destination IP */
        addr.type = NETADDR_IPv4;
        addr.len = IPv4_ADDR_LEN;
        memcpy(addr.addr, ip->dst, addr.len);
        netaddrsprintf(strA, &addr);
        /* strB = source IP */
        memcpy(addr.addr, ip->src, addr.len);
        netaddrsprintf(strB, &addr);
        /* strC = protocol */
        strC[0] = '\0';
        switch (ip->proto)
        {
        case IPv4_PROTO_ICMP:
            sprintf(strC, "ICMP");
            break;
        case IPv4_PROTO_TCP:
            sprintf(strC, "TCP");
            break;
        case IPv4_PROTO_UDP:
            sprintf(strC, "UDP");
            break;
        }
        /* Output summary */
        printf("IP %s > %s : %s", strB, strA, strC);
        break;
    }
    printf("\n");

    /* Print verbose */
    if (verbose > SNOOP_VERBOSE_NONE)
    {
        snoopPrintEthernet(ether, verbose);
        switch (net2hs(ether->type))
        {
        case ETHER_TYPE_ARP:
            snoopPrintArp(arp, verbose);
            break;
        case ETHER_TYPE_IPv4:
            snoopPrintIpv4(ip, verbose);
            appHdr = (uchar *)ip + ((ip->ver_ihl & IPv4_IHL) * 4);
            switch (ip->proto)
            {
            case IPv4_PROTO_TCP:
                snoopPrintTcp((struct tcpPkt *)appHdr, verbose);
                break;
            case IPv4_PROTO_UDP:
                snoopPrintUdp((struct udpPkt *)appHdr, verbose);
                break;
            }
            break;
        }
    }

    /* Print dump */
    if ((SNOOP_DUMP_HEX == dump) || (SNOOP_DUMP_CHAR == dump))
    {
        for (i = 0; i < pkt->len; i += 16)
        {
            printf("\t0x%04X  ", i);
            for (j = i; j < i + 16; j++)
            {
                if (0 == j % 2)
                {
                    printf(" ");
                }
                if (j >= pkt->len)
                {
                    printf("  ");
                    continue;
                }
                printf("%02X", pkt->data[j]);
            }
            if (SNOOP_DUMP_CHAR == dump)
            {
                printf("  |");
                for (j = i; (j < i + 16 && j < pkt->len); j++)
                {
                    ch = pkt->data[j];
                    if (!isascii(ch) || iscntrl(ch))
                    {
                        ch = '.';
                    }
                    printf("%c", ch);
                }
                printf("|");
            }
            printf("\n");
        }
    }

    return OK;
}
