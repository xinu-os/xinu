/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <interrupt.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdlib.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Delete a TCB.
 * @param tcbptr pointer to transmission control block for connection
 * @precondition TCB mutex is already held
 */
devcall tcpFree(struct tcb *tcbptr)
{
    irqmask im;
    semaphore temp;

    /* Verify TCB is not already free */
    if (TCP_CLOSED == tcbptr->state)
    {
        signal(tcbptr->mutex);
        return SYSERR;
    }

    TCP_TRACE("Free TCB");

    im = disable();

    /* Free TCB */
    temp = tcbptr->mutex;
    semfree(tcbptr->openclose);
    semfree(tcbptr->readers);
    semfree(tcbptr->writers);
    tcpTimerPurge(tcbptr, NULL);
    bzero(tcbptr, sizeof(struct tcb));  /* Clear tcp structure. */
    tcbptr->state = TCP_CLOSED;
    tcbptr->devstate = TCP_FREE;
    tcbptr->mutex = temp;
    restore(im);
    signal(tcbptr->mutex);
    return OK;
}
