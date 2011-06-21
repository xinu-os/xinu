/**
 * @file recvclr.c
 * @provides recvclr.
 *
 * $Id: recvclr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Clear messages, return waiting message (if any)
 * @return msg if available, NOMSG if no message
 */
message recvclr(void)
{
    register struct thrent *thrptr;
    irqmask im;
    message msg;

    im = disable();
    thrptr = &thrtab[thrcurrent];
    if (thrptr->hasmsg)
    {
        msg = thrptr->msg;
    }                           /* retrieve message       */
    else
    {
        msg = NOMSG;
    }
    thrptr->hasmsg = FALSE;     /* reset message flag   */
    restore(im);
    return msg;
}
