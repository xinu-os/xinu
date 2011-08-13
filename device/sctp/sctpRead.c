/**
 * @file     sctpRead.c
 * @provides sctpRead
 *
 * $Id: sctpRead.c 2083 2009-10-06 19:53:29Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <sctp.h>

/**
 * Copy data chunks from SCTP socket to user buffer (blocks if no data
 * available).
 * @param devptr SCTP device entry pointer
 * @param buf user buffer
 * @param len bytes available in `buf`
 * @return amount of data copied into buffer (0 if socket is closed)
 */
devcall sctpRead(device *devptr, void *buf, uint len)
{
	uint count = 0;

    return count;
}
