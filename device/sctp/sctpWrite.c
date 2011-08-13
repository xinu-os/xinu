/**
 * @file     sctpWrite.c
 * @provides sctpWrite
 *
 * $Id: sctpWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <sctp.h>

/**
 * Copy data from user buffer to SCTP socket.
 * @param devptr SCTP device entry pointer
 * @param buf buffer of data to be sent
 * @param len length of data to be sent
 * @return amount of successfully sent
 */
devcall sctpWrite(device *devptr, void *buf, uint len)
{
	uint count = 0;
    return count;
}
