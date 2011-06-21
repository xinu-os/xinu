/*
 * @file emuCorrupt.c
 * @provides emuCorrupt
 *
 * $Id: emuCorrupt.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>
#include <route.h>

/**
 * Corrupts packets as specified by user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuCorrupt(struct packet *pkt)
{

    return rtRecv(pkt);
}
