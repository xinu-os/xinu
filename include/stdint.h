/**
 * @file stdint.h
 *
 * Signed and unsigned integer types of exact width.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _STDINT_H_
#define _STDINT_H_

typedef signed char          int8_t;
typedef short                int16_t;
typedef int                  int32_t;
typedef long long            int64_t;

typedef unsigned char        uint8_t;
typedef unsigned short       uint16_t;
typedef unsigned int         uint32_t;
typedef unsigned long long   uint64_t;

#if __LP64__
typedef long int             intptr_t;
typedef unsigned long int    uintptr_t;
#else
typedef int                  intptr_t;
typedef unsigned int         uintptr_t;
#endif

#endif /* _STDINT_H_ */
