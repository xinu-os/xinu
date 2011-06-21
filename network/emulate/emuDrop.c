/*
 * @file emuDrop.c
 * @provides emuDrop
 *
 * $Id: emuDrop.c 2122 2009-11-11 01:06:21Z kthurow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdlib.h>
#include <network.h>
#include <netemu.h>
#include <route.h>
#include <date.h>
#include <stdio.h>
#include <udp.h>

/**
 * Drop packets based on user settings
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuDrop(struct packet *pkt)
{
    /* TODO: make this user specifiable */
    short dropPercent = 0;

    /* drop packet when random value < percent to drop */
    if ((rand() % 100) < dropPercent)
    {
        RT_TRACE("Dropped by Emulator: %d\n");
        netFreebuf(pkt);
        return OK;
    }

    return emuDelay(pkt);
}
