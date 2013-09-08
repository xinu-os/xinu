/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt and exception
 * processing.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <system/arch/mips/interrupt.h>

/** Address of interrupt request handler.                          */
#define IRQ_ADDR      0x80000180
/** Address of Xinu-defined trap (exception) vector.               */
#define TRAPVEC_ADDR  0x80000200
/** Address of Xinu-defined interrupt request vector.              */
#define IRQVEC_ADDR   0x80000280
/** Address of end of Xinu-defined interrupt tables.               */
#define IRQVEC_END    0x800002E0

#ifndef __ASSEMBLER__
void enable_irq(irqmask);
void disable_irq(irqmask);
#endif

#endif  /* _INTERRUPT_H_ */
