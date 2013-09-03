/**
 * @file watchdog.h
 *
 * Interface to the system watchdog timer.  This is a platform-dependent feature
 * and may not be available on some platforms.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <stddef.h>

syscall watchdogset(uint msecs);

#endif /* _WATCHDOG_H_ */
