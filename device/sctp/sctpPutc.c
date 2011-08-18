/**
 * @file sctpPutc.c
 * @provides sctpPutc
 *
 * $Id: sctpPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <sctp.h>

/**
 * Write a single octet to SCTP device.
 * @param devptr SCTP device entry pointer
 * @param ch character to output
 * @return count of characters written
 */
devcall sctpPutc(device *devptr, char ch)
{
    return sctpWrite(devptr, &ch, 1);
}
