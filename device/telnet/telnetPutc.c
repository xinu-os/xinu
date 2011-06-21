/**
 * @file telnetPutc.c
 * @provides telnetPutc.
 *
 * $Id: telnetPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <telnet.h>
#include <device.h>

/**
 * Write a single character to TELNET.
 * @param devptr TELNET device table entry
 * @param ch character to output
 * @return OK if character was written sucessfully, otherwise SYSERR
 */
devcall telnetPutc(device *devptr, char ch)
{
    if (SYSERR == telnetWrite(devptr, &ch, 1))
    {
        return SYSERR;
    }

    return OK;
}
