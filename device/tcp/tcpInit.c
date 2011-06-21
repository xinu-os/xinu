/**
 * @file tcpInit.c
 * @provides tcpInit
 *
 * $Id: tcpInit.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdlib.h>
#include <tcp.h>

struct tcb tcptab[NTCP];

/**
 * Initialize TCP structures.
 * @param devptr tcp device table entry
 * @return OK if device is initialized
 */
devcall tcpInit(device *devptr)
{
    struct tcb *tcbptr;

    tcbptr = &tcptab[devptr->minor];
    bzero(tcbptr, sizeof(struct tcb));
    tcbptr->state = TCP_CLOSED;
    tcbptr->devstate = TCP_FREE;
    tcbptr->mutex = semcreate(1);
    if (SYSERR == (int)tcbptr->mutex)
    {
        return SYSERR;
    }

    return OK;
}
