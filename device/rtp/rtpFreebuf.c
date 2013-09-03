/**
 * @file     rtpFreebuf.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>
#include <rtp.h>

syscall rtpFreebuf(struct rtpPkt *rtppkt)
{
    return buffree(rtppkt);
}
