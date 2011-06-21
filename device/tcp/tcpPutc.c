/**
 * @file tcpPutc.c
 * @provides tcpPutc
 *
 * $Id: tcpPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

/**
 * Write a single octet to send via TCP.
 * @param devptr TCP device table entry
 * @param ch character to output
 * @return count of characters written
 */
devcall tcpPutc(device *devptr, char ch)
{
    return tcpWrite(devptr, &ch, 1);
}
