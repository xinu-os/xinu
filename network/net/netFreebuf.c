/**
 * file netGetbuf.c
 * @provides netFreebuf
 * 
 * $Id: netFreebuf.c 2079 2009-10-05 22:32:48Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <network.h>

/**
 * Frees a buffer for storing a packet.
 * @return OK if successful, SYSERR if an error occured
 */
syscall netFreebuf(struct packet *pkt)
{
    return buffree(pkt);
}
