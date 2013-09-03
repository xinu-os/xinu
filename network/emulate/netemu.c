/*
 * @file netemu.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <netemu.h>

/**
 * @ingroup netemu
 *
 * Process a packet through the network emulator
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall netemu(struct packet *pkt)
{

    return emuDrop(pkt);
}
