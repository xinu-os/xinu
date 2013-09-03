/**
 * @file tcpTimerTrigger.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
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
