/**
 * @file kexec.h
 *
 * Kernel execute: transfer control to a new kernel.  This is intended to be
 * used to allow Xinu to act as a bootloader, e.g. so that a new Xinu kernel can
 * be executed without rebooting the hardware.
 *
 * This is a platform-dependent system call and may not be available on a given
 * platform.  Furthermore, the new kernel must be valid to execute on the given
 * platform.  This may include being linked to run at a certain address.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _KEXEC_H_
#define _KEXEC_H_

#include <stddef.h>

syscall kexec(const void *kernel, uint size);

#endif
