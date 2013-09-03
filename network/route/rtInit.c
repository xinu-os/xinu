/**
 * @file rtInit.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <mailbox.h>
#include <network.h>
#include <route.h>
#include <stdlib.h>
#include <thread.h>

struct rtEntry rttab[RT_NENTRY];
mailbox rtqueue;

/**
 * @ingroup route
 *
 * Initialize the route table and route daemon.
 * @return OK if initialization is successful, otherwise SYSERR
 */
syscall rtInit(void)
{
    int i = 0;

    /* Initialize route table */
    for (i = 0; i < RT_NENTRY; i++)
    {
        bzero(&rttab[i], sizeof(struct rtEntry));
        rttab[i].state = RT_FREE;
    }

    /* Initialize route queue */
    rtqueue = mailboxAlloc(RT_NQUEUE);
    if (SYSERR == rtqueue)
    {
        return SYSERR;
    }

    /* Spawn rtDameon thread */
    ready(create
          ((void *)rtDaemon, RT_THR_STK, RT_THR_PRIO, "rtDaemon", 0),
          RESCHED_NO);

    return OK;
}
