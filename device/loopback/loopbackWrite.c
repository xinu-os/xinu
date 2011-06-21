/**
 * @file     loopbackWrite.c
 * @provides loopbackWrite.
 *
 * $Id: loopbackWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>

/**
 * Write a character to the loopback. 
 * @param devptr 
 * @param buf buffer of characters to output
 * @param len size of the buffer
 * @return count of characters output
 */
devcall loopbackWrite(device *devptr, void *buf, uint len)
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
        /* If char wasn't put into buffer return SYSERR */
        if (SYSERR == loopbackPutc(devptr, buffer[i]))
        {
            return i;
        }
    }

    return i;
}
