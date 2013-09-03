/**
 * @file icmpDaemon.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <icmp.h>
#include <ipv4.h>
#include <mailbox.h>
#include <interrupt.h>

/**
 * @ingroup icmp
 *
 * ICMP daemon to manage ping replies.
 */
thread icmpDaemon(void)
{
#if NNETIF
    struct packet *pkt = NULL;
    struct ipv4Pkt *ip = NULL;
    struct netaddr dst;
    int i = 0, match = -1;

    while (TRUE)
    {
        if (NULL != pkt)
        {
            netFreebuf(pkt);
        }

        pkt = (struct packet *)mailboxReceive(icmpqueue);
        ICMP_TRACE("Daemon received ICMP packet");
        if (SYSERR == (int)pkt)
        {
            continue;
        }

        ip = (struct ipv4Pkt *)pkt->nethdr;
        dst.type = NETADDR_IPv4;
        dst.len = IPv4_ADDR_LEN;
        memcpy(dst.addr, ip->dst, dst.len);

        ICMP_TRACE("NNETIF = %d", NNETIF);
        // Scan the net interface table for an address match.
        for (i = 0; i < NNETIF; i++)
        {
            if (NET_ALLOC != netiftab[i].state)
                continue;

            // Check for our interface addresses and broadcast addresses
            if (netaddrequal(&netiftab[i].ip, &dst)
                || (netaddrequal(&netiftab[i].ipbrc, &dst)))
            {
                ICMP_TRACE("Packet matches netif %d", i);
                match = i;
                break;
            }
        }

        if ((match >= 0) && (match < NNETIF))
        {
            ICMP_TRACE("Daemon replying");
            icmpEchoReply(pkt, &netiftab[match]);
        }
        else
        {
            ICMP_TRACE("Daemon discarding");
            netFreebuf(pkt);
        }

    }
#endif                          /* NNETIF */
    return OK;
}
