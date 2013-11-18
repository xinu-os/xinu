/**
 * @file icmpDaemon.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <icmp.h>
#include <ipv4.h>
#include <mailbox.h>

/**
 * @ingroup icmp
 *
 * Daemon that responds to ICMP echo requests (pings).
 */
thread icmpDaemon(void)
{
#if NNETIF

    while (TRUE)
    {
        struct packet *pkt;

        /* Received the next ICMP Echo Request.  */
        pkt = (struct packet *)mailboxReceive(icmpqueue);
        ICMP_TRACE("Daemon received ICMP packet");
        ICMP_TRACE("%u bytes total; %u bytes ICMP header+data",
                   pkt->len, pkt->len - (pkt->curr - pkt->data));

        /* Send the ICMP Echo Reply, re-using the packet buffer.  */
        if (OK != icmpEchoReply(pkt))
        {
            ICMP_TRACE("Failed to send ICMP Echo Reply.");
        }

        /* Free the packet buffer.  */
        netFreebuf(pkt);
    }
#endif /* NNETIF */
    return OK;
}
