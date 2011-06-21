/**
 * @file tlb.h
 * Definitions relating to the hardware TLB.
 *
 * $Id: tlb.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _TLB_H_
#define _TLB_H_

#define TLB_SIZE 32

#define TLB_EXC_START  (void *)0x80000000 /**< TLB entry vector        */
#define TLB_EXC_LENGTH (32 * 4)           /**< 32, 4-byte instructions */

#ifndef __ASSEMBLER__

void tlbInit(void);
void tlbMissHandler(int, int *);
void dumptlb(void);

#endif                          /* __ASSEMBLER__ */

/* Offsets for TLB handler activation record */
#define TLBREC_PTE  16
#define TLBREC_AT   24
#define TLBREC_V0   28
#define TLBREC_V1   32
#define TLBREC_A0   36
#define TLBREC_A1   40
#define TLBREC_A2   44
#define TLBREC_A3   48
#define TLBREC_S0   52
#define TLBREC_S1   56
#define TLBREC_S2   60
#define TLBREC_S3   64
#define TLBREC_S4   68
#define TLBREC_S5   72
#define TLBREC_S6   76
#define TLBREC_S7   80
#define TLBREC_S8   84
#define TLBREC_S9   88
#define TLBREC_RA   92
#define TLBREC_SIZE 96

#endif                          /* _TLB_H_ */
