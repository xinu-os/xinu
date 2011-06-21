/**
 * @file device.h 
 * @provides isbaddev.
 *
 * Contains all definitions relating to the Xinu device subsystem.
 *
 * $Id: device.h 2020 2009-08-13 17:50:08Z mschul $
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
devcall write(int, void *, uint);
devcall getc(int);
devcall putc(int, char);
devcall seek(int, uint);
devcall control(int, int, long, long);
syscall getdev(char *);

#endif                          /* _DEVICE_H_ */
