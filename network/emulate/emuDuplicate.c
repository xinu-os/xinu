/*
 * @file emuDuplicate.c
 * @provides emuDuplicate
 *
 * $Id: emuDuplicate.c 2126 2009-11-12 07:23:04Z svn $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <stdlib.h>
#include <netemu.h>

//int test(struct packet *pkt, int dupes);
/**
 * Duplicate packets as specified by user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
syscall emuDuplicate(struct packet *pkt)
{
    /*
       short dupePercent = 100;
       int dupes = 2;

       if ((rand() % 100) < dupePercent)
       {
       return test(pkt, dupes);
       }
     */
    return emuReorder(pkt);
}

/*
syscall test(struct packet *pkt, int dupes)
{
    if (dupes == 1)
    {
        return emuReorder(pkt);
    }

    return (emuReorder(pkt) | test(pkt, dupes - 1));
}*/
