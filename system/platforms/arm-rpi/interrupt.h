/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt handling.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stddef.h>

typedef interrupt (*interrupt_handler_t)(void);

extern interrupt_handler_t interruptVector[];

typedef unsigned long irqmask;  /**< machine status for disable/restore  */


void enable(void);
irqmask disable(void);
irqmask restore(irqmask);
void enable_irq(irqmask);
void disable_irq(irqmask);
void memory_barrier(void);

/* We're using the the output compare register in the BCM2835 system timer with
 * index 3.  This corresponds to "GPU" IRQ 3, although this is not actually
 * documented by the Broadcom docs.  */
#define IRQ_TIMER 3

/* TODO: Remove useless IRQ_HW* definitions.  */
#define IRQ_HW0 0
#define IRQ_HW5 5

#endif /* _INTERRUPT_H_ */
