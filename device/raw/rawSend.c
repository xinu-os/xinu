/**
 * @file     rawSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <network.h>
#include <raw.h>
#include <route.h>

/**
 * @ingroup raw
 *
 * Send an outgoing packet of protocol other than UDP or TCP.
 * @param rawptr pointer to the raw socket control block
 * @param buf buffer to semd
 * @param len size of the buffer
 * @return OK if packet was sent succesfully, otherwise SYSERR
 */
syscall rawSend(struct raw *rawptr, void *buf, uint len)
{
    struct packet *pkt;
    struct rtEntry *rtptr;
    int result;

    /* Error check pointers */
    if ((NULL == rawptr) || (NULL == buf) || (RAW_ALLOC != rawptr->state))
    {
        RAW_TRACE("Invalid args");
        return SYSERR;
    }

    /* Verify necessary fields are specified */
    if (rawptr->flags & RAW_OHDR)
    {
        /* Verify localip and remoteip is specified */
        if ((NULL == rawptr->localip.type)
            || (NULL == rawptr->remoteip.type))
        {
            RAW_TRACE("Under-specified socket");
            return SYSERR;
        }
    }
    else
    {
        /* Verify remoteip and protocol is specified */
        if ((NULL == rawptr->proto) || (NULL == rawptr->remoteip.type))
        {
            RAW_TRACE("Under-specified socket");
            return SYSERR;
        }
    }

    /* Get buffer for packet */
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        RAW_TRACE("Failed to get buffer");
        return SYSERR;
    }

    /* Place buffer in packet */
    pkt->curr -= len;
    pkt->len += len;
    memcpy(pkt->curr, buf, len);

    /* If network layer header is already included, call netSend */
    if (rawptr->flags & RAW_OHDR)
    {
        pkt->nethdr = pkt->curr;

        /* Lookup destination in route table */
        rtptr = rtLookup(&rawptr->remoteip);
        if ((NULL == rtptr) || (NULL == rtptr->nif))
        {
            RAW_TRACE("No route");
            netFreebuf(pkt);
            return SYSERR;
        }

        /* Packet has next hop in route table */
        pkt->nif = rtptr->nif;
        RAW_TRACE("Send via link layer");
        result = netSend(pkt, NULL, &rawptr->remoteip,
                         rawptr->remoteip.type);
    }
    else
    {
        /* Call appropriate function to add network layer header */
        switch (rawptr->remoteip.type)
        {
        case NETADDR_IPv4:
            RAW_TRACE("Send via IPv4");
            result = ipv4Send(pkt, &rawptr->localip, &rawptr->remoteip,
                              rawptr->proto);
            break;
        default:
            result = SYSERR;
        }
    }

    if (SYSERR == netFreebuf(pkt))
    {
        return SYSERR;
    }
    return result;
}
