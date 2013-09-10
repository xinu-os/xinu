/**
 * @file compiler.h
 *
 * Compiler-specific definitions.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _COMPILER_H_
#define _COMPILER_H_

/** Packed attribute:
 * Annotate a structure with this to lay it out with no padding bytes.  */
#define __packed                    __attribute__((packed))

/** Format attribute:
 * Provide compiler warnings about bad printf()-like format strings.  */
#define __printf_format(fmt, vargs) __attribute__((format(printf, fmt, vargs)))

/** Type of expression:
 * typeof(x) evaluates to the C type of x.  */
#define typeof __typeof__

/** Static assertion:
 * Assert that a predicate is true at compilation time.  This generates no code
 * in the resulting binary.  */
#define STATIC_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))

/** Aligned attribute:
 * Annotate a member or buffer with this to align it on the specified byte
 * boundary.  */
#define __aligned(n)                __attribute__((aligned(n)))

#endif /* _COMPILER_H_ */
