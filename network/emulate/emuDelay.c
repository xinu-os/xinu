/*
 * @file emuDelay.c
 * @provides emuDelay
 *
 * $Id: emuDelay.c 2126 2009-11-12 07:23:04Z svn $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <stdlib.h>
#include <thread.h>
#include <netemu.h>

/**
 * Delay packets as specified by user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuDelay(struct packet *pkt)
{
    /*
       short delayTime = 100;
       short delayPercent = 100;

       if ((rand() % 100) < delayPercent)
       {
       sleep(delayTime);
       }
     */
    return emuDuplicate(pkt);
}
