/**
 * @file tcpPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Write a single octet to send via TCP.
 *
 * @param devptr
 *      TCP device table entry
 *
 * @param ch
 *      character to output
 *
 * @return
 *      @p ch as an <code>unsigned char</code> cast to an @c int on success; @c
 *      SYSERR on failure.
 */
devcall tcpPutc(device *devptr, char ch)
{
    int ret;

    ret = tcpWrite(devptr, &ch, 1);
    if (ret == 1)
    {
        return (uchar)ch;
    }
    else
    {
        return SYSERR;
    }
}
