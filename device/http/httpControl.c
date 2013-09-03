/**
 * @file httpControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Control function for HTTP pseudo devices.
 * @param devptr HTTP device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall httpControl(device *devptr, int func, long arg1, long arg2)
{
    struct http *webptr;
    char old;

    /* Setup and error check pointer to structure */
    webptr = &httptab[devptr->minor];
    if (HTTP_STATE_ALLOC != webptr->state)
    {
        return SYSERR;
    }

    switch (func)
    {
        /* Set input flag: arg1 = flag to set       */
        /* return old value of flag                 */
    case HTTP_CTRL_SET_FLAG:
        old = webptr->flags & arg1;
        webptr->flags |= (arg1);
        return old;

        /* Clear input flag: arg1 = flag to clear   */
        /* return old value of flag                 */
    case HTTP_CTRL_CLR_FLAG:
        old = webptr->flags & arg1;
        webptr->flags &= ~(arg1);
        return old;
    }

    return SYSERR;
}
