/**
 * @file     loopbackRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>

/**
 * @ingroup loopback
 *
 * Read up to the specified number of characters from a loopback.
 *
 * @param devptr 
 *      Loopback device to read from.
 * @param buf
 *      Buffer into which to place the read data.
 * @param len
 *      Maximum number of characters to read.
 *
 * @return
 *      Returns the number of characters read, which may be less than @p len if
 *      @c EOF or a read error occurs.   Alternatively, if the loopback device
 *      is not open, returns @c SYSERR.
 */
devcall loopbackRead(device *devptr, void *buf, uint len)
{
    struct loopback *lbkptr = NULL;
    uint i;
    unsigned char *buffer = buf;
    int ret;

    lbkptr = &looptab[devptr->minor];

    /* Check if loopback is open */
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
        return SYSERR;
    }

    for (i = 0; i < len; i++)
    {
        ret = loopbackGetc(devptr);
        if (ret == EOF)
        {
            break;
        }
        buffer[i] = ret;
    }

    return i;
}
