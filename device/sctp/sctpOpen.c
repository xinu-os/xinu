/**
 * @file     sctpOpen.c
 * @provides sctpOpen
 *
 * $Id: sctpOpen.c 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <bufpool.h>
#include <device.h>
#include <network.h>
#include <sctp.h>
#include <stdarg.h>
#include <interrupt.h>

/**
 * Open SCTP socket on local device.
 * @param devptr SCTP device entry pointer
 * @param ap 2nd argument is the local IP address (required)
 *           3rd argument is the local port (NULL for auto-assign)
 *           4th argument is the remote IP address (NULL for none)
 *           5th argument is the remote port (NULL for none)
 * @return OK if SCTP opened properly, otherwise SYSERR
 */
devcall sctpOpen(device *devptr, va_list ap)
{
    return OK;
}

