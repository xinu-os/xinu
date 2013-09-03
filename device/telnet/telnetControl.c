/**
 * @file telnetControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <telnet.h>
#include <tty.h>

/**
 * @ingroup telnet
 *
 * Control function for TELNET pseudo devices.
 * @param devptr TELNET device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall telnetControl(device *devptr, int func, long arg1, long arg2)
{
    struct telnet *tntptr;
    device *phw;

    /* Setup and error check pointers to structures */
    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    switch (func)
    {
    case TELNET_CTRL_FLUSH:
        telnetFlush(devptr);
        return OK;
    case TELNET_CTRL_CLRFLAG:
        /* arg1 is the flag we are clearing */
        tntptr->flags &= ~arg1;
        return OK;
    case TELNET_CTRL_SETFLAG:
        /* arg1 is the flag we are setting */
        tntptr->flags |= arg1;
        return OK;
    case TTY_CTRL_SET_IFLAG:
        return OK;
    case TTY_CTRL_CLR_IFLAG:
        return OK;
    case TTY_CTRL_SET_OFLAG:
        return OK;
    case TTY_CTRL_CLR_OFLAG:
        return OK;
    }

    return SYSERR;
}
