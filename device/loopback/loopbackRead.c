/**
 * @file     loopbackRead.c
 * @provides loopbackRead.
 *
 * $Id: loopbackRead.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>

/**
 * Read characters from a loopback.
 * @param devptr 
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, SYSERR if closed
 */
devcall loopbackRead(device *devptr, void *buf, uint len)
{
    struct loopback *lbkptr = NULL;
    uint i;
    char *buffer = buf;

    lbkptr = &looptab[devptr->minor];

    /* Check if loopback is open */
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
        return SYSERR;
    }

    for (i = 0; i < len; i++)
    {
        buffer[i] = loopbackGetc(devptr);
    }

    return i;
}
