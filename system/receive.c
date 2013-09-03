/**
 * @file receive.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * receive - wait for a message and return it
 * @return message
 */
message receive(void)
{
    register struct thrent *thrptr;
    irqmask im;
    message msg;

    im = disable();
    thrptr = &thrtab[thrcurrent];
    if (FALSE == thrptr->hasmsg)
    {                           /* if no message, wait for one */
        thrptr->state = THRRECV;
        resched();
    }
    msg = thrptr->msg;          /* retrieve message                */
    thrptr->hasmsg = FALSE;     /* reset message flag              */
    restore(im);
    return msg;
}
