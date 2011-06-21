/**
 * @file rtDaemon.c
 * @provides rtDaemon
 *
 * $Id: rtDaemon.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <mailbox.h>
#include <network.h>
#include <route.h>
#include <thread.h>

/**
 * Route dameon to route packets
 */
thread rtDaemon(void)
{
    struct packet *pkt = NULL;

    enable();

    while (TRUE)
    {
        pkt = (struct packet *)mailboxReceive(rtqueue);
        RT_TRACE("Daemon received packet");
        if (SYSERR == (int)pkt)
        {
            RT_TRACE("Daemon received packet has an error");
            continue;
        }

        rtSend(pkt);
        if (SYSERR == netFreebuf(pkt))
        {
            RT_TRACE("Failed to free packet buffer");
            continue;
        }
    }

    return OK;
}
