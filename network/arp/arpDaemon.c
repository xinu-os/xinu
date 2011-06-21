/**
 * @file arpDaemon.c
 * @provides arpDaemon
 *
 * $Id: arpDaemon.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <mailbox.h>

/**
 * ARP daemon to manage the ARP table.
 */
thread arpDaemon(void)
{
    struct packet *pkt = NULL;

    while (TRUE)
    {
        pkt = (struct packet *)mailboxReceive(arpqueue);
        ARP_TRACE("Daemon received ARP packet");
        if (SYSERR == (int)pkt)
        {
            continue;
        }

        arpSendReply(pkt);

        /* Free buffer for the packet */
        if (SYSERR == netFreebuf(pkt))
        {
            ARP_TRACE("Failed to free packet buffer");
            continue;
        }
    }

    return OK;
}
