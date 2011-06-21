/**
 * @file kernel.h 
 *
 * The base include file for the Xinu kernel. Defines symbolic constants,
 * universal return constants, intialization constants, machine size
 * definitions, inline utility functions, and include types
 *
 * $Id: kernel.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <conf.h>
#include <stddef.h>

/* Kernel function prototypes */
int nulluser(void);

/* Kernel function prototypes */
syscall kprintf(char *fmt, ...);
syscall kputc(device *, uchar);
syscall kgetc(device *);

#endif                          /* _KERNEL_H_ */
