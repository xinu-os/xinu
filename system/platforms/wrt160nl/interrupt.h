/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt and exception
 * processing.
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/** Address of interrupt request handler.                          */
#define IRQ_ADDR      0x80000180
/** Address of Xinu-defined trap (exception) vector.               */
#define TRAPVEC_ADDR  0x80001000
/** Address of Xinu-defined interrupt request vector.              */
#define IRQVEC_ADDR   0x80001080
/** Address of end of Xinu-defined interrupt tables.               */
#define IRQVEC_END    0x800010C0

#ifndef __ASSEMBLER__
void enable_irq(irqmask);
void disable_irq(irqmask);
#endif

#endif  /* _INTERRUPT_H_ */
