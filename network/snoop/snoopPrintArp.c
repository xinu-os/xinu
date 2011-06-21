/**
 * @file snoopPrintArp.c
 * @provides snoopPrintArp
 * 
 * $Id: snoopPrintArp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <network.h>
#include <snoop.h>
#include <stdio.h>

/**
 * Print contents of ARP packet
 * @param arp ARP packet
 * @return OK if print successful, SYSERR if error occurs
 */
int snoopPrintArp(struct arpPkt *arp, char verbose)
{
    char output[40];
    struct netaddr hwaddr;
    struct netaddr praddr;

    /* Error check pointer */
    if (NULL == arp)
    {
        return SYSERR;
    }

    if (verbose >= SNOOP_VERBOSE_ONE)
    {
        printf(" ----- ARP Header -----\n", "");
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            switch (net2hs(arp->hwtype))
            {
            case ARP_HWTYPE_ETHERNET:
                sprintf(output, "Ethernet");
                break;
            default:
                sprintf(output, "0x%04X", net2hs(arp->hwtype));
                break;
            }
            printf("  Hw type: %-14s ", output);
            switch (net2hs(arp->prtype))
            {
            case ARP_PRTYPE_IPv4:
                sprintf(output, "IPv4");
                break;
            default:
                sprintf(output, "0x%04X", net2hs(arp->prtype));
                break;
            }
            printf("Proto type: %-11s\n", output);

            sprintf(output, "%d bytes", arp->hwalen);
            printf("  Hw len: %-15s ", output);
            sprintf(output, "%d bytes", arp->pralen);
            printf("Proto len: %-12s ", output);
        }

        switch (net2hs(arp->op))
        {
        case ARP_OP_RQST:
            sprintf(output, "Request");
            break;
        case ARP_OP_REPLY:
            sprintf(output, "Reply");
            break;
        default:
            sprintf(output, "0x%04X", net2hs(arp->op));
            break;
        }
        printf("Operation: %-12s\n", output);

        hwaddr.type = net2hs(arp->hwtype);
        hwaddr.len = arp->hwalen;
        memcpy(hwaddr.addr, &arp->addrs[ARP_ADDR_SHA(arp)], hwaddr.len);
        netaddrsprintf(output, &hwaddr);
        printf("  Src hw addr: %-22s ", output);
        praddr.type = net2hs(arp->prtype);
        praddr.len = arp->pralen;
        memcpy(praddr.addr, &arp->addrs[ARP_ADDR_SPA(arp)], praddr.len);
        netaddrsprintf(output, &praddr);
        printf("Src proto addr: %-19s\n", output);

        memcpy(hwaddr.addr, &arp->addrs[ARP_ADDR_DHA(arp)], hwaddr.len);
        netaddrsprintf(output, &hwaddr);
        printf("  Dst hw addr: %-22s ", output);
        memcpy(praddr.addr, &arp->addrs[ARP_ADDR_DPA(arp)], praddr.len);
        netaddrsprintf(output, &praddr);
        printf("Dst proto addr: %-19s\n", output);
    }

    return OK;
}
