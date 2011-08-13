/**
 * @file        sctpClose.c
 * @provides    sctpClose
 *
 * $Id
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <stdlib.h>
#include <sctp.h>
#include <interrupt.h>

/**
 * Close and clear a SCTP device
 * @param devptr SCTP device entry pointer
 * @return OK if SCTP is closed properly, otherwise SYSERR
 */
devcall sctpClose(device *devptr)
{
    return OK;
}
