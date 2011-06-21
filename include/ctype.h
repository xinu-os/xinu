/**
 * @file ctype.h
 * 
 * Includes isalpha, isupper, islower, isdigit, isspace, ispunct, isalnum,
 * isprint, iscntrl, toupper, tolower, toascii, iseof
 *
 * $Id: ctype.h 2072 2009-09-21 22:26:50Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _CTYPE_H_
#define _CTYPE_H_

#define _U 0x01
#define _L 0x02
#define _N 0x04
#define _S 0x08
#define _P 0x10
#define _C 0x20
#define _X 0x40

extern const char _ctype_[];

#define isalpha(c)   ((_ctype_+1)[(int)c]&(_U|_L))
#define isupper(c)   ((_ctype_+1)[(int)c]&_U)
#define islower(c)   ((_ctype_+1)[(int)c]&_L)
#define isdigit(c)   ((_ctype_+1)[(int)c]&_N)
#define isxdigit(c)  ((_ctype_+1)[(int)c]&(_N|_X))
#define isspace(c)   ((_ctype_+1)[(int)c]&_S)
#define ispunct(c)   ((_ctype_+1)[(int)c]&_P)
#define isalnum(c)   ((_ctype_+1)[(int)c]&(_U|_L|_N))
#define isprint(c)   ((_ctype_+1)[(int)c]&(_P|_U|_L|_N|_S))
#define iscntrl(c)   ((_ctype_+1)[(int)c]&_C)
#define isascii(c)   ((unsigned)((int)(c))<=0x7F)

#define toupper(c)   (((int)(c))-'a'+'A')
#define tolower(c)   (((int)(c))-'A'+'a')
#define toascii(c)   (((int)(c))&0x7F)

#define iseof(c)     (0x04 == (int)c)

#endif                          /* _CTYPE_H_ */
