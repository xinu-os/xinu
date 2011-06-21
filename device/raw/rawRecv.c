/**
 * @file     rawRecv.c
 * @provides rawRecv
 *
 * $Id: rawRecv.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <icmp.h>
#include <raw.h>

/**
 * Process an incoming protocol other than UDP or TCP.
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall rawRecv(struct packet *pkt, struct netaddr *src,
                struct netaddr *dst, ushort proto)
{
    struct raw *rawptr;
    uint index;

    /* Error check pointers */
    if ((NULL == pkt) || (NULL == src) || (NULL == dst))
    {
        return SYSERR;
    }

    /* Locate raw socket for the packet */
    rawptr = rawDemux(src, dst, proto);
    if (NULL == rawptr)
    {
        RAW_TRACE("No matching socket");
        /* Send ICMP port unreachable message */
        icmpDestUnreach(pkt, ICMP_PORT_UNR);
        netFreebuf(pkt);
        return OK;
    }

    /* Ensure there is space */
    if (rawptr->icount >= RAW_IBLEN)
    {
        netFreebuf(pkt);
        return SYSERR;
    }

    /* Enqueue packet */
    index = (rawptr->istart + rawptr->icount) % RAW_IBLEN;
    rawptr->in[index] = pkt;
    netaddrcpy(&rawptr->src[index], src);
    rawptr->icount++;
    signal(rawptr->isema);

    RAW_TRACE("Enqueued packet");
    return OK;
}
