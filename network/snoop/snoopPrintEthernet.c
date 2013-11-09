/**
 * @file snoopPrintEthernet.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ethernet.h>
#include <network.h>
#include <snoop.h>
#include <stdio.h>

/**
 * @ingroup snoop
 *
 * Print contents of Ethernet packet
 * @param ether Ethernet packet
 * @return OK if print successful, SYSERR if error occurs
 */
int snoopPrintEthernet(struct etherPkt *ether, char verbose)
{
    char output[40];
    struct netaddr hwaddr;

    /* Error check pointer */
    if (NULL == ether)
    {
        return SYSERR;
    }

    if (verbose >= SNOOP_VERBOSE_TWO)
    {
        /* Print ethernet header */
        printf(" ----- Ethernet Header -----\n");
        hwaddr.type = NETADDR_ETHERNET;
        hwaddr.len = ETH_ADDR_LEN;
        memcpy(hwaddr.addr, ether->dst, hwaddr.len);
        netaddrsprintf(output, &hwaddr);
        printf("  Dst: %-25s ", output);
        memcpy(hwaddr.addr, ether->src, hwaddr.len);
        netaddrsprintf(output, &hwaddr);
        printf("Src: %-25s ", output);
        switch (net2hs(ether->type))
        {
        case ETHER_TYPE_IPv4:
            sprintf(output, "IP");
            break;
        case ETHER_TYPE_ARP:
            sprintf(output, "ARP");
            break;
        default:
            sprintf(output, "0x%04X", net2hs(ether->type));
            break;
        }
        printf("Type: %-5s\n", output);
    }

    return OK;
}
