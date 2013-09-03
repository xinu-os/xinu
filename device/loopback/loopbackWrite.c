/**
 * @file     loopbackWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <loopback.h>

/**
 * @ingroup loopback
 *
 * Write data to the loopbock device.
 *
 * @param devptr 
 *      Pointer to the loopback device to write to.
 * @param buf
 *      Buffer of data to write.
 * @param len
 *      Number of bytes of data to write.
 *
 * @return
 *      On success, returns the number of bytes written, which may be less than
 *      @p len in the event of a write error.  Alternatively, @c SYSERR is
 *      returned if the loopback device is not open or if a write error occurred
 *      before any data at all was successfully written.
 */
devcall loopbackWrite(device *devptr, const void *buf, uint len)
{
    struct loopback *lbkptr = NULL;
    uint i;
    const uchar *buffer = buf;

    lbkptr = &looptab[devptr->minor];

    /* Check if loopback is open */
    if (LOOP_STATE_ALLOC != lbkptr->state)
    {
        return SYSERR;
    }

    for (i = 0; i < len; i++)
    {
        if (SYSERR == loopbackPutc(devptr, buffer[i]))
        {
            if (i == 0)
            {
                i = SYSERR;
            }
            break;
        }
    }

    return i;
}
