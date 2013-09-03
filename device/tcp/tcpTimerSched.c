/**
 * @file tcpTimerSched.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <stddef.h>
#include <tcp.h>

static int allocEvent(void);

/**
 * @ingroup tcp
 *
 * Schedule TCP timer events.
 * @param time milliseconds before timer triggers
 * @param tcbptr TCB with which event is associated
 * @param type type of timer event
 * @return OK if event is successfully scheduled, otherwise SYSERR
 */
devcall tcpTimerSched(int time, struct tcb *tcbptr, uchar type)
{
    int evt = 0;
    struct tcpEvent *evtptr = NULL;
    struct tcpEvent *prev = NULL;
    struct tcpEvent *next = NULL;

    /* Verify parameters */
    if ((time < 0) || (NULL == tcbptr))
    {
        return SYSERR;
    }

    wait(tcpmutex);
    /* Setup timer event */
    evt = allocEvent();
    if (SYSERR == evt)
    {
        signal(tcpmutex);
        return SYSERR;
    }
    evtptr = &tcptimertab[evt];
    evtptr->time = time;
    evtptr->type = type;
    evtptr->tcbptr = tcbptr;

    /* Insert event into delta queue */
    prev = &tcptimertab[TCP_EVT_HEAD];
    next = prev->next;
    while ((next != NULL) && (next->remain <= time))
    {
        time -= next->remain;
        prev = next;
        next = next->next;
    }
    evtptr->next = next;
    prev->next = evtptr;
    evtptr->remain = time;
    if (next != NULL)
    {
        next->remain -= time;
    }
    signal(tcpmutex);

    return OK;
}

/**
 * Allocate an unused timer event block
 * @return index to used block, or SYSERR if none exist
 * @precondition TCP Timer mutex is already held
 * @postcondition TCP Timer mutex is still held
 */
static int allocEvent(void)
{
    int evt;
    static int nextevt = 0;

    /* Check all TCP timer event slots (except the head) */
    for (evt = 0; evt < TCP_NEVENTS; evt++)
    {
        nextevt = (nextevt + 1) % TCP_NEVENTS;
        /* Ignore head */
        if (TCP_EVT_HEAD == nextevt)
        {
            continue;
        }
        if (FALSE == tcptimertab[nextevt].used)
        {
            tcptimertab[nextevt].used = TRUE;
            return nextevt;
        }
    }
    return SYSERR;

}
