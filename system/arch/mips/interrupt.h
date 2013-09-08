/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt and exception
 * processing, common to MIPS architecture.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _MIPS_INTERRUPT_H_
#define _MIPS_INTERRUPT_H_


#ifndef __ASSEMBLER__
typedef unsigned long irqmask;  /**< machine status for disable/restore  */

irqmask disable(void);
irqmask restore(irqmask);
irqmask enable(void);
irqmask enable_cpuirq(irqmask);
irqmask disable_cpuirq(irqmask);
void exlreset(void);
void exlset(void);

/**
 * Definitions to allow C array manipulation of vectors.
 * The cast below makes the following a pointer to a table of
 * pointers to functions which take no parameters (void) and return
 * nothing (void).
 */
#define exceptionVector ((interrupt (**)(void))TRAPVEC_ADDR)
#define interruptVector ((interrupt (**)(void))IRQVEC_ADDR)

#endif /* __ASSEMBLER__ */

/* Indices for exception code                                     */
#define EXC_INT        0   /**< Interrupt                          */
#define EXC_MOD        1   /**< Store on Read-Only Page            */
#define EXC_TLBL       2   /**< No valid TLB entry matches         */
#define EXC_TLBS       3   /**< Standard TLB miss                  */
#define EXC_ADEL       4   /**< Address error (load)               */
#define EXC_ADES       5   /**< Address error (store)              */
#define EXC_IBE        6   /**< Instruction Bus Error              */
#define EXC_DBE        7   /**< Data Bus Error                     */
#define EXC_SYS        8   /**< System call                        */
#define EXC_BP         9   /**< Breakpoint exception               */
#define EXC_RI        10   /**< Reserved instruction               */
#define EXC_CPUN      11   /**< Coprocessor Unusable               */
#define EXC_OVER      12   /**< Arithmetic Overflow                */
#define EXC_TRAP      13   /**< Trap Exception                     */
#define EXC_MDMX      22   /**< MDMX unusable                      */
#define EXC_WATCH     23   /**< Reference to WatchHi/WatchLo       */
#define EXC_MCHECK    24   /**< Machine Check                      */
#define EXC_CACHEERR  30   /**< Cache Error / Debug Mode           */

/* Offsets for the MIPS interrupt context record.                  */
#define IRQREC_ZER    16
#define IRQREC_AT     20
#define IRQREC_V0     24
#define IRQREC_V1     28
#define IRQREC_A0     32
#define IRQREC_A1     36
#define IRQREC_A2     40
#define IRQREC_A3     44
#define IRQREC_T0     48
#define IRQREC_T1     52
#define IRQREC_T2     56
#define IRQREC_T3     60
#define IRQREC_T4     64
#define IRQREC_T5     68
#define IRQREC_T6     72
#define IRQREC_T7     76
#define IRQREC_S0     80
#define IRQREC_S1     84
#define IRQREC_S2     88
#define IRQREC_S3     92
#define IRQREC_S4     96
#define IRQREC_S5     100
#define IRQREC_S6     104
#define IRQREC_S7     108
#define IRQREC_T8     112
#define IRQREC_T9     116
#define IRQREC_K0     120
#define IRQREC_K1     124
#define IRQREC_S8     128
#define IRQREC_SP     132
#define IRQREC_S9     136
#define IRQREC_RA     140
#define IRQREC_LO     144
#define IRQREC_HI     148
#define IRQREC_CAUSE  152
#define IRQREC_STATUS 156
#define IRQREC_EPC    164
#define IRQREC_SIZE   168

/* Indices for interrupt request vector  */
#define IRQ_SW0       0
#define IRQ_SW1       1
#define IRQ_HW0       2
#define IRQ_HW1       3
#define IRQ_HW2       4
#define IRQ_HW3       5
#define IRQ_HW4       6
#define IRQ_HW5       7

#endif /* _MIPS_INTERRUPT_H_ */
