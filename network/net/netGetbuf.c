/**
 * file netGetbuf.c
 * @provides netGetbuf
 * 
 * $Id: netGetbuf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <stdlib.h>

/**
 * Provides a buffer for storing a packet.
 * @return pointer to a packet buffer, SYSERR if an error occured
 */
struct packet *netGetbuf(void)
{
    struct packet *pkt = NULL;          /**< pointer to packet            */

    /* Obtain a buffer for the packet */
    pkt = bufget(netpool);
    if (SYSERR == (int)pkt)
    {
        return (struct packet *)SYSERR;
    }

    bzero(pkt, sizeof(struct packet) + NET_MAX_PKTLEN);

    /* Initialize packet buffer */
    pkt->nif = NULL;
    pkt->len = 0;
    /* Initialize curr to point to end of buffer */
    pkt->curr = pkt->data + NET_MAX_PKTLEN;

    return pkt;
}
