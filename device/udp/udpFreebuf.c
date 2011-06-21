/**
 * @file     udpFreebuf.c
 * @provides udpFreebuf
 *
 * $Id: udpFreebuf.c 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <udp.h>

syscall udpFreebuf(struct udpPkt *udppkt)
{
    return buffree(udppkt);
}
