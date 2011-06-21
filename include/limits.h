/**
 * @file limits.h
 * 
 * $Id: limits.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _LIMITS_H_
#define _LIMITS_H_

#define CHAR_BIT  8             /**< bits in a char                     */
#define CHAR_MAX  127           /**< maximum value of char              */
#define CHAR_MIN  (-128)        /**< minimum value of char              */
#define SCHAR_MAX 127           /**< maximum value of signed char       */
#define SCHAR_MIN (-128)        /**< minimum value of signed char       */
#define UCHAR_MAX 255           /**< maximum value of unsigned char     */

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
