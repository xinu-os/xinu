/**
 * @file ttyControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <tty.h>

/**
 * @ingroup tty
 *
 * Control function for TTY pseudo devices.
 * @param devptr TTY device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall ttyControl(device *devptr, int func, long arg1, long arg2)
{
    struct tty *ttyptr;
    device *phw;
    char old;

    /* Setup and error check pointers to structures */
    ttyptr = &ttytab[devptr->minor];
    phw = ttyptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    switch (func)
    {
        /* Set input flags: arg1 = flags to set      */
        /* return old value of flags                 */
    case TTY_CTRL_SET_IFLAG:
        old = ttyptr->iflags & arg1;
        ttyptr->iflags |= (arg1);
        return old;

        /* Clear input flags: arg1 = flags to clear  */
        /* return old value of flags                 */
    case TTY_CTRL_CLR_IFLAG:
        old = ttyptr->iflags & arg1;
        ttyptr->iflags &= ~(arg1);
        return old;

        /* Set output flags: arg1 = flags to set     */
        /* return old value of flags                 */
    case TTY_CTRL_SET_OFLAG:
        old = ttyptr->oflags & arg1;
        ttyptr->oflags |= (arg1);
        return old;

        /* Clear output flags: arg1 = flags to clear */
        /* return old value of flags                 */
    case TTY_CTRL_CLR_OFLAG:
        old = ttyptr->oflags & arg1;
        ttyptr->oflags &= ~(arg1);
        return old;
    }

    return SYSERR;
}
