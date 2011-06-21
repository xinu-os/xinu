/**
 * @file debug.h
 *
 * $Id: debug.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stddef.h>

#define DEBUG_ASCII 0x01
#define DEBUG_HEX   0x02

void hexdump(void *buffer, ulong length, bool text);
void debugbreak(void);
void debugret(void);

#endif                          /* _DEBGU_H_ */
