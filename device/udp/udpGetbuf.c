/**
 * @file     udpGetbuf.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <stdlib.h>
#include <udp.h>

/**
 * @ingroup udpinternal
 */
struct udpPkt *udpGetbuf(struct udp *udpptr)
{
    struct udpPkt *udppkt = NULL;

    udppkt = bufget(udpptr->inPool);
    if (SYSERR == (int)udppkt)
    {
        return (struct udpPkt *)SYSERR;
    }

    bzero(udppkt, NET_MAX_PKTLEN);

    return udppkt;
}
