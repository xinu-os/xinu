/**
 * @file ctype.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <stddef.h>

#define _U 0x01
#define _L 0x02
#define _N 0x04
#define _S 0x08
#define _P 0x10
#define _C 0x20
#define _X 0x40

extern const unsigned char _ctype_[256];

/* NOTE: for simplicity we diverge from the C standard in the below functions by
 * always interpreting the argument as an 'unsigned char', thereby not allowing
 * EOF to be passed.  This also guards against common buggy code where a plain
 * 'char' is passed to these functions.  */

/** @addtogroup libxc
 *  @{ */
#define isalpha(c)   ((_ctype_)[(unsigned char)c] & (_U|_L))
#define isupper(c)   ((_ctype_)[(unsigned char)c] & _U)
#define islower(c)   ((_ctype_)[(unsigned char)c] & _L)
#define isdigit(c)   ((_ctype_)[(unsigned char)c] & _N)
#define isxdigit(c)  ((_ctype_)[(unsigned char)c] & (_N|_X))
#define isspace(c)   ((_ctype_)[(unsigned char)c] & _S)
#define ispunct(c)   ((_ctype_)[(unsigned char)c] & _P)
#define isalnum(c)   ((_ctype_)[(unsigned char)c] & (_U|_L|_N))
#define isprint(c)   ((_ctype_)[(unsigned char)c] & (_P|_U|_L|_N|_S))
#define iscntrl(c)   ((_ctype_)[(unsigned char)c] & _C)
#define isascii(c)   ((unsigned char)(c) <= 0x7F)

#define toupper(c)   (islower((unsigned char)(c)) ? \
                        (unsigned char)(c) + ('A' - 'a') : (unsigned char)(c))
#define tolower(c)   (isupper((unsigned char)(c)) ? \
                            (unsigned char)(c) + ('a' - 'A') : (unsigned char)(c))
#define toascii(c)   (((unsigned char)(c)) & 0x7F)
/** @} */

#endif /* _CTYPE_H_ */
