/**
 * @file     sctpInit.c
 * @provides sctpInit
 *
 * $Id: sctpInit.c 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdlib.h>
#include <sctp.h>

struct sctp_tcb sctptab[NSCTP];

/**
 * Prepare available SCTP devices for use during system initialization.
 * @param devptr SCTP device entry pointer
 * @return OK unless initialization fails.
 */
devcall sctpInit(device *devptr)
{
    return OK;
}
