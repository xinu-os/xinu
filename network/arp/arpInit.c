/**
 * @file arpInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <mailbox.h>
#include <stdlib.h>
#include <thread.h>

struct arpEntry arptab[ARP_NENTRY];
mailbox arpqueue;

/**
 * @ingroup arp
 *
 * Initializes ARP table and ARP daemon.
 * @return OK if initialization is successful, otherwise SYSERR
 */
syscall arpInit(void)
{
    int i = 0;

    /* Initialize ARP table */
    for (i = 0; i < ARP_NENTRY; i++)
    {
        bzero(&arptab[i], sizeof(struct arpEntry));
        arptab[i].state = ARP_FREE;
    }

    /* Initialize ARP queue */
    arpqueue = mailboxAlloc(ARP_NQUEUE);
    if (SYSERR == arpqueue)
    {
        return SYSERR;
    }

    /* Spawn arpDaemon thread */
    ready(create
          ((void *)arpDaemon, ARP_THR_STK, ARP_THR_PRIO, "arpDaemon", 0),
          RESCHED_NO);

    return OK;
}
