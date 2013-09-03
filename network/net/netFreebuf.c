/**
 * file netGetbuf.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>

/**
 * @ingroup network
 *
 * Frees a buffer for storing a packet.
 * @return OK if successful, SYSERR if an error occured
 */
syscall netFreebuf(struct packet *pkt)
{
    return buffree(pkt);
}
