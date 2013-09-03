/**
 * @file stdarg.h
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _STDARG_H_
#define _STDARG_H_

/* GCC-specific varargs */
typedef __builtin_va_list va_list;

#define va_copy(dst, src)	__builtin_va_copy(dst, src)
#define va_start(last, va)	__builtin_va_start(last, va)
#define va_arg(va, type)	__builtin_va_arg(va, type)
#define va_end(va)		__builtin_va_end(va)

#endif                          /* _STDARG_H_ */
