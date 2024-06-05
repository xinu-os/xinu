/**
 * @file interrupt.h
 *
 * Constants and declarations associated with interrupt handling.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */
 
#ifndef _RISCV_INTERRUPT_H_
#define _RISCV_INTERRUPT_H_

 
#include <stddef.h>

typedef interrupt (*interrupt_handler_t)(void);

extern interrupt_handler_t interruptVector[];

typedef unsigned long irqmask;  /**< machine status for disable/restore  */


void enable(void);
irqmask disable(void);
irqmask restore(irqmask);
void enable_irq(irqmask);
void disable_irq(irqmask);

#endif /* _RISCV_INTERRUPT_H_ */
