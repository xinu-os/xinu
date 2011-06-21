/*
 * @file emuReorder.c
 * @provides emuReorder
 *
 * $Id: emuReorder.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>

/**
 * Reorder packets as specified by the user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuReorder(struct packet *pkt)
{

    return emuCorrupt(pkt);
}
