/**
 * @file device.h 
 *
 * Contains all definitions relating to the Xinu device subsystem.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/* Device table declarations */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <conf.h>
#include <stddef.h>

/**
 * Check that device id is between 0 and NDEVS. 
 *
 * @param f id number to test
 */
#define isbaddev(f)  ( !(0 <= (f) && (f) < NDEVS) )

/* Standard driver functions */
devcall open(int, ...);
devcall close(int);
devcall read(int, void *, uint);
devcall write(int, const void *, uint);
devcall getc(int);
devcall putc(int, char);
devcall seek(int, uint);
devcall control(int, int, long, long);
syscall getdev(const char *);

#endif                          /* _DEVICE_H_ */
