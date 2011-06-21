/**
 * @file dsp.h
 *
 * $Id: dsp.h 2111 2009-10-30 19:24:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _DSP_H_
#define _DSP_H_

unsigned char linear2ulaw(int);
int ulaw2linear(unsigned char);

#endif                          /* _DSP_H_ */
