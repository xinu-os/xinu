/**
 * @file     sctpControl.c
 * @provides sctpControl
 *
 * $Id: sctpControl.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <network.h>
#include <sctp.h>

/**
 * Control function for SCTP devices.
 * @param devptr SCTP device entry pointer
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall sctpControl(device *devptr, int func, long arg1, long arg2)
{
    return OK;
}
