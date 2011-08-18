/**
 * @file sctpGetc.c
 * @provides sctpGetc
 *
 * $Id: sctpGetc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <sctp.h>

/**
 * Read a single octet from SCTP.
 * @param devptr SCTP device entry pointer
 * @return character read from SCTP
 */
devcall sctpGetc(device *devptr)
{
    uchar ch = '\0';
    return ch;
}
