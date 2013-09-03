/**
 * @file rawControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <raw.h>
#include <interrupt.h>

/**
 * @ingroup raw
 *
 * Control function for raw sockets.
 * @param devptr RAW device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall rawControl(device *devptr, int func, long arg1, long arg2)
{
    struct raw *rawptr;
    uchar old;
    irqmask im;

    rawptr = &rawtab[devptr->minor];
    im = disable();
    /* Check if raw socket is open */
    if (RAW_ALLOC != rawptr->state)
    {
        restore(im);
        return SYSERR;
    }

    switch (func)
    {
        /* Set flags: arg1 = flags to set      */
        /* return old value of flags                 */
    case RAW_CTRL_SETFLAG:
        old = rawptr->flags & arg1;
        rawptr->flags |= (arg1);
        restore(im);
        return old;

        /* Clear flags: arg1 = flags to clear  */
        /* return old value of flags                 */
    case RAW_CTRL_CLRFLAG:
        old = rawptr->flags & arg1;
        rawptr->flags &= ~(arg1);
        restore(im);
        return old;
    }

    restore(im);
    return SYSERR;
}
