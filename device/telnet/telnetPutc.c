/**
 * @file telnetPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <telnet.h>
#include <device.h>

/**
 * @ingroup telnet
 *
 * Write a single character to TELNET.
 *
 * @param devptr
 *      TELNET device table entry
 * @param ch
 *      character to output
 *
 * @return
 *      @p ch as an <code>unsigned char</code> cast to an @c int on success; @c
 *      SYSERR on failure.
 */
devcall telnetPutc(device *devptr, char ch)
{
    int ret;

    ret = telnetWrite(devptr, &ch, 1);
    if (ret == 1)
    {
        return (uchar)ch;
    }
    else
    {
        return SYSERR;
    }
}
