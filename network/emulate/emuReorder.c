/*
 * @file emuReorder.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>

/**
 * @ingroup netemu
 *
 * Reorder packets as specified by the user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuReorder(struct packet *pkt)
{

    return emuCorrupt(pkt);
}
