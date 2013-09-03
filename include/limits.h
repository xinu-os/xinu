/**
 * @file limits.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _LIMITS_H_
#define _LIMITS_H_

#define CHAR_BIT  8             /**< bits in a char                     */

#define SCHAR_MAX 127           /**< maximum value of signed char       */
#define SCHAR_MIN (-128)        /**< minimum value of signed char       */

#define UCHAR_MAX 255           /**< maximum value of unsigned char     */

/* gcc will automatically define __CHAR_UNSIGNED__ if it treats 'char' as
 * unsigned on the current platform.  By default, char is signed on most
 * platforms, but unsigned on ARM.  */
#ifdef __CHAR_UNSIGNED__
#  define CHAR_MAX  UCHAR_MAX   /**< maximum value of char              */
#  define CHAR_MIN  0           /**< minimum value of char              */
#else
#  define CHAR_MAX  SCHAR_MAX   /**< maximum value of char              */
#  define CHAR_MIN  SCHAR_MIN   /**< minimum value of char              */
#endif

#define INT_MAX   2147483647    /**< maximum value of int               */
#define INT_MIN   (-INT_MAX-1)  /**< minimum value of int               */
#define UINT_MAX  (2U*INT_MAX+1) /**< maximum value of unsigned int     */

#define LONG_MAX  2147483647    /**< maximum value of long              */
#define LONG_MIN  (-LONG_MAX-1) /**< minimum value of long              */
#define ULONG_MAX (2UL*LONG_MAX+1) /**< maximum value of unsigned long  */

#define SHRT_MAX  32767         /**< maximum value of short             */
#define SHRT_MIN  (-SHRT_MAX-1) /**< minimum value of short             */
#define USHRT_MAX  65535        /**< maximum value of unsigned short    */

#endif                          /* _LIMITS_H_ */
