/**
 * @file tcpTimerPurge.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Remove all TCP timer events for a particular TCB.
 * @param tcbptr TCB for which to remove all events
 * @param type type of events to remove, all types are removed if NULL
 * @return the time elapsed for the first remvoed event, SYSERR if no 
 * 		events were removed
 */
devcall tcpTimerPurge(struct tcb *tcbptr, uchar type)
{
    struct tcpEvent *prev = NULL;
    struct tcpEvent *cur = NULL;
    int result = SYSERR;

    wait(tcpmutex);
    prev = &tcptimertab[TCP_EVT_HEAD];
    cur = prev->next;
    while (cur != NULL)
    {
        if ((cur->tcbptr == tcbptr)
            && ((NULL == type) || (cur->type == type)))
        {
            if (SYSERR == result)
            {
                result = cur->time - cur->remain;
            }
            if (cur->next != NULL)
            {
                cur->next->remain += cur->remain;
            }
            prev->next = cur->next;
            cur->used = FALSE;
        }
        prev = cur;
        cur = cur->next;
    }
    signal(tcpmutex);

    return result;
}
