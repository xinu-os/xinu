/*
 * @file emuCorrupt.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>
#include <route.h>

/**
 * @ingroup netemu
 *
 * Corrupts packets as specified by user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuCorrupt(struct packet *pkt)
{

    return rtRecv(pkt);
}
