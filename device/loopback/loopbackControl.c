/**
 * @file     loopbackControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>

/**
 * @ingroup loopback
 *
 * Control function for loopback devices.
 * @param devptr device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall loopbackControl(device *devptr, int func, long arg1, long arg2)
{
    struct loopback *lbkptr = NULL;
    int old;

    lbkptr = &looptab[devptr->minor];

    /* Check if loopback is open */
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
        return SYSERR;
    }

    switch (func)
    {
    case LOOP_CTRL_SET_FLAG:
        old = lbkptr->flags;
        lbkptr->flags |= arg1;
        return old;

    case LOOP_CTRL_CLR_FLAG:
        old = lbkptr->flags & arg1;
        lbkptr->flags &= ~(ulong)arg1;
        return old;

    default:
        return SYSERR;
    }

    return OK;
}
