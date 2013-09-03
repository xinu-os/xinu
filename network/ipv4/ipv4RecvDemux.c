/* 
 * file ipv4RecvDemux.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>

/**
 *  Determines if an IP packet is destined for one of our network interfaces.
 *  @param dst destination IP address
 *  @return TRUE If destined for one of our netifs, otherwise FALSE
 */
bool ipv4RecvDemux(struct netaddr *dst)
{
#if NNETIF
    int i;
    struct netif *netptr;

    /* Check every interface */
    for (i = 0; i < NNETIF; i++)
    {
        netptr = &netiftab[i];

        /* Skip closed interfaces */
        if (netptr->state != NET_ALLOC)
        {
            continue;
        }

        /* Check if destination IP matches IP for interface */
        if (netaddrequal(dst, &netptr->ip)
            || netaddrequal(dst, &netptr->ipbrc))
        {
            return TRUE;
        }
    }
#endif                          /* NNETIF */
    return FALSE;
}
