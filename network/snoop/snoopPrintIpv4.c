/**
 * @file snoopPrintIpv4.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>
#include <snoop.h>
#include <stdio.h>

/**
 * @ingroup snoop
 *
 * Print contents of an IPv4 packet
 * @param ip IPv4 packet
 * @return OK if print successful, SYSERR if error occurs
 */
int snoopPrintIpv4(struct ipv4Pkt *ip, char verbose)
{
    char output[40];
    struct netaddr praddr;

    /* Error check pointer */
    if (NULL == ip)
    {
        return SYSERR;
    }

    if (verbose >= SNOOP_VERBOSE_ONE)
    {
        printf(" ----- IPv4 Header -----\n");
        /* Version */
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            printf("  Version: %-14d ", (ip->ver_ihl & IPv4_VER) >> 4);
        }
        else
        {
            printf("  ");
        }
        /* Header length */
        sprintf(output, "%d bytes", ((ip->ver_ihl & IPv4_IHL) * 4));
        printf("Header len: %-11s ", output);
        /* Service */
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            switch (ip->tos)
            {
            case IPv4_TOS_ROUTINE:
                sprintf(output, "Routine");
                break;
            case IPv4_TOS_PRIO:
                sprintf(output, "Priority");
                break;
            case IPv4_TOS_IM:
                sprintf(output, "Immediate");
                break;
            case IPv4_TOS_FLASH:
                sprintf(output, "Flash");
                break;
            case IPv4_TOS_FLASHOV:
                sprintf(output, "Flash Override");
                break;
            case IPv4_TOS_CRITIC:
                sprintf(output, "CRITIC/ECP");
                break;
            case IPv4_TOS_INTCNTRL:
                sprintf(output, "Interntwk Ctrl");
                break;
            case IPv4_TOS_NETCNTRL:
                sprintf(output, "Network Ctrl");
                break;
            default:
                sprintf(output, "0x%08X", ip->tos);
                break;
            }
            printf("Service: %-14s\n", output);
        }
        /* Total length */
        sprintf(output, "%u bytes", net2hs(ip->len));
        printf("  Total len: %-12s ", output);
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            /* Identification */
            printf("Identification: %-7u ", net2hs(ip->id));
            /* Flags */
            sprintf(output, "0x%03X", net2hs(ip->flags_froff) >> 13);
            printf("Flags: %-16s\n", output);
            /* Fragment offset */
            sprintf(output, "%d bytes",
                    (net2hs(ip->flags_froff) & IPv4_FROFF));
            printf("  Frag offset: %-10s ", output);
            /* Time to live */
            sprintf(output, "%d hops", ip->ttl);
            printf("Time to Live: %-9s ", output);
        }
        /* protocol */
        switch (ip->proto)
        {
        case IPv4_PROTO_ICMP:
            sprintf(output, "ICMP");
            break;
        case IPv4_PROTO_UDP:
            sprintf(output, "UDP");
            break;
        case IPv4_PROTO_TCP:
            sprintf(output, "TCP");
            break;
        default:
            sprintf(output, "%d", ip->proto);
            break;
        }
        printf("Protocol: %-13s\n", output);
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            /* Checksum */
            /*      printf("  Header checksum: 0x%04X ", net2hs(ip->chksm));
               temp = ip->chksm;
               ip->chksm = 0;
               ip->chksm = ipChecksum((uchar *)ip, (4 * (ip->ver_ihl & IPv4_IHL)));
               if (ip->chksm == temp)
               {
               printf("(correct)\n");
               }
               else
               {
               printf("(incorrect, correct: 0x%04X)\n", net2hs(ip->chksm));
               } */
        }

        praddr.type = NETADDR_IPv4;
        praddr.len = IPv4_ADDR_LEN;
        memcpy(praddr.addr, ip->src, praddr.len);
        netaddrsprintf(output, &praddr);
        printf("  Src addr: %-25s ", output);
        memcpy(praddr.addr, ip->dst, praddr.len);
        netaddrsprintf(output, &praddr);
        printf("Dst addr: %-25s\n", output);
    }

    return OK;
}
