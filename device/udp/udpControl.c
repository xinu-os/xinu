/**
 * @file     udpControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <network.h>
#include <udp.h>

/**
 * @ingroup udpexternal
 *
 * Control function for udp devices.
 * @param devptr udp device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall udpControl(device *devptr, int func, long arg1, long arg2)
{
    struct udp *udpptr;
    uchar old;

    udpptr = &udptab[devptr->minor];

    switch (func)
    {
    case UDP_CTRL_ACCEPT:
        /* arg1 is port and arg2 is pointer to netaddr */
        udpptr->localpt = arg1;
        if (NULL == arg2)
        {
            return SYSERR;
        }
        else
        {
            netaddrcpy(&(udpptr->localip), (struct netaddr *)arg2);
        }
        return OK;
    case UDP_CTRL_BIND:
        /* arg1 is port and arg2 is pointer to netaddr */
        udpptr->remotept = arg1;
        if (NULL == arg2)
        {
            bzero(&(udpptr->remoteip), sizeof(struct netaddr));
        }
        else
        {
            netaddrcpy(&(udpptr->remoteip), (struct netaddr *)arg2);
        }
        return OK;
    case UDP_CTRL_CLRFLAG:
        /* arg1 is the flag we are clearing */
        old = udpptr->flags & arg1;
        udpptr->flags &= ~arg1;
        return old;
    case UDP_CTRL_SETFLAG:
        /* arg1 is the flag we are setting */
        old = udpptr->flags & arg1;
        udpptr->flags |= arg1;
        return old;
    default:
        return SYSERR;
    }

    return OK;
}
