/**
 * @file tcpTimerTrigger.c
 * @provides tcpTimerTrigger
 *
 * $Id: tcpTimerTrigger.c 2076 2009-09-24 23:05:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <stddef.h>
#include <tcp.h>

/**
 * Processes TCP timer events.
 * @param evtptr timer event to process
 */
void tcpTimerTrigger(uchar type, struct tcb *tcbptr)
{
    switch (type)
    {
    case TCP_EVT_TIMEWT:
        wait(tcbptr->mutex);
        tcpFree(tcbptr);
        return;
    case TCP_EVT_RXT:
        tcpSendRxt(tcbptr);
        return;
    case TCP_EVT_PERSIST:
        tcpSendPersist(tcbptr);
        return;
    }
}
