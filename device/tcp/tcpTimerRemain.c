/**
 * @file tcpTimerRemain.c
 * @provides tcpTimerRemain
 *
 * $Id: tcpTimerRemain.c 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <stddef.h>
#include <tcp.h>

/**
 * Determine time reamining for a TCP timer event for a particular TCB.
 * @param tcbptr TCB to which event corresponds
 * @param type type of timer event
 * @return time remaining for event, 0 if no event exists
 */
int tcpTimerRemain(struct tcb *tcbptr, uchar type)
{
    struct tcpEvent *cur = NULL;
    int time = 0;

    wait(tcpmutex);
    cur = tcptimertab[TCP_EVT_HEAD].next;
    while (cur != NULL)
    {
        time += cur->remain;
        if ((cur->tcbptr == tcbptr) && (cur->type == type))
        {
            signal(tcpmutex);
            return time;
        }
        cur = cur->next;
    }
    signal(tcpmutex);

    return 0;
}
