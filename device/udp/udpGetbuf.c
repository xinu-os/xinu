/**
 * @file     udpGetbuf.c
 * @provides udpGetbuf
 *
 * $Id: udpGetbuf.c 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <stdlib.h>
#include <udp.h>

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
