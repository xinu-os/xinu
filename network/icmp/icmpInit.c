/**
 * @file icmpInit.c
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mailbox.h>
#include <icmp.h>
#include <stdlib.h>
#include <thread.h>

mailbox icmpqueue;
struct icmpEchoQueue echotab[NPINGQUEUE];

/**
 * @ingroup icmp
 *
 * Initializes ICMP daemon.
 * @return OK if initialization is successful, otherwise SYSERR
 */
syscall icmpInit(void)
{
    int i;

    /* Initialize ICMP queue */
    icmpqueue = mailboxAlloc(ICMP_NQUEUE);
    if (SYSERR == icmpqueue)
    {
        return SYSERR;
    }

    bzero(echotab, sizeof(echotab));
    for (i = 0; i < NPINGQUEUE; i++)
    {
        echotab[i].tid = BADTID;
    }

    /* Spawn icmpDaemon thread */
    ready(create
          ((void *)icmpDaemon, ICMP_THR_STK, ICMP_THR_PRIO, "icmpDaemon",
           0), RESCHED_NO);

    return OK;
}
