/**
 * @file rtRecv.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <mailbox.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 */
syscall rtRecv(struct packet *pkt)
{
    irqmask im;

    /* Error check pointers */
    if (NULL == pkt)
    {
        return SYSERR;
    }

    /* If route queue is full, then drop packet */
    im = disable();
    if (mailboxCount(rtqueue) >= RT_NQUEUE)
    {
        restore(im);
        RT_TRACE("Route queue full");
        netFreebuf(pkt);
        return OK;
    }

    /* Place packet in queue */
    if (SYSERR == mailboxSend(rtqueue, (int)pkt))
    {
        restore(im);
        RT_TRACE("Failed to enqueue packet");
        netFreebuf(pkt);
        return SYSERR;
    }

    restore(im);
    RT_TRACE("Enqueued packet for routing");
    return OK;
}
