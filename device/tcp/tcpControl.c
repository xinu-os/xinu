/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

/**
 * Control function for TCP devices.
 * @param devptr ethernet device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall tcpControl(device *devptr, int func, long arg1, long arg2)
{
    struct tcb *tcbptr;
    uint bytes;

    tcbptr = &tcptab[devptr->minor];

    wait(tcbptr->mutex);
    switch (func)
    {
        /* Get number of bytes sent */
    case TCP_CTRL_SENTBYTES:
        bytes = tcbptr->obytes;
        signal(tcbptr->mutex);
        return bytes;

        /* Get number of bytes received */
    case TCP_CTRL_RECVBYTES:
        bytes = tcbptr->ibytes;
        signal(tcbptr->mutex);
        return bytes;

        /* Unrecongnized control function */
    default:
        signal(tcbptr->mutex);
        return SYSERR;
    }

    signal(tcbptr->mutex);
    return SYSERR;
}
