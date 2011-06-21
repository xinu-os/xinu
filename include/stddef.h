/**
 * @file stddef.h
 *
 * Includes standard type definitions and primary constants used across
 * all aspects of Xinu.
 *
 * $Id: stddef.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _STDDEF_H_
#define _STDDEF_H_

/* Base type definitions */
typedef unsigned char uchar;    /**< unsigned char type                 */
typedef unsigned short ushort;  /**< unsigned short type                */
typedef unsigned int uint;      /**< unsigned int type                  */
typedef unsigned long ulong;    /**< unsigned long type                 */
typedef char bool;              /**< boolean type                       */

/* Function declaration return types */
typedef int syscall;            /**< system call declaration            */
typedef int devcall;            /**< device call declaration            */
typedef int shellcmd;           /**< shell command declaration          */
typedef int thread;             /**< thread declaration                 */
typedef void interrupt;         /**< interrupt procedure                */
typedef void exchandler;        /**< exception procedure                */
typedef int message;            /**< message passing content            */

typedef int tid_typ;            /**< thread ID type                     */

/* Boolean type and constants */
#define FALSE        0          /**< boolean false                      */
#define TRUE         1          /**< boolean true                       */

/* Universal return constants */
#define OK        1             /**< system call ok                     */
#define NULL      0             /**< null pointer for linked lists      */
#define SYSERR   (-1)           /**< system call failed                 */
#define EOF      (-2)           /**< End-of-file (usually from read)    */
#define TIMEOUT  (-3)           /**< system call timed out              */
#define NOMSG    (-4)           /**< no message to receive              */

#endif                          /* _STDDEF_H_ */
