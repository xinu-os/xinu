/*
 * @file netemu.c
 * @provides netemu
 *
 * $Id: netemu.c 2118 2009-11-05 05:22:51Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>

/**
 * Process a packet through the network emulator
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall netemu(struct packet *pkt)
{

    return emuDrop(pkt);
}
