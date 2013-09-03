/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt and exception
 * processing.
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stddef.h>

#define	IRQBASE 32 /* base ivec for IRQ0                  */
#define IRQ0     0 /* programmable interval timer         */
#define IRQ1     1 /* keyboard controller                 */
#define IRQ2     2 /* unknown                             */
#define IRQ3     3 /* serial port 2                       */
#define IRQ4     4 /* serial port 1                       */
#define IRQ5     5 /* hard disk controller                */
#define IRQ6     6 /* floppy disk controller              */
#define IRQ7     7 /* parallel port / spurious interrupts */

#define	IGDT_TASK	 5	/* task gate IDT descriptor       */
#define	IGDT_INTR	14	/* interrupt gate IDT descriptor  */
#define	IGDT_TRAPG	15	/* Trap Gate                      */

#define	NID  48

#ifndef __ASSEMBLER__

typedef unsigned long irqmask;  /**< machine status for disable/restore  */

/** Tables of exception/interrupt vectors */
extern interrupt (*exctab[NID])(void);

/* Interrrupt enabling function prototypes */
irqmask disable(void);
irqmask restore(irqmask);
irqmask enable(void);
irqmask enable_irq(irqmask);
irqmask disable_irq(irqmask);

/* Interrupt configuration function prototypes */
void init_evec(void);
void set_evec(int, int);
void set_handler(int, interrupt (*)(void));
void dispatch(int, int *);

#endif /* __ASSEMBLER__ */

struct idt
{
	unsigned short	igd_loffset;
	unsigned short	igd_segsel;
	unsigned int	igd_rsvd : 5;
	unsigned int	igd_mbz : 3;
	unsigned int	igd_type : 5;
	unsigned int	igd_dpl : 2;
	unsigned int	igd_present : 1;
	unsigned short	igd_hoffset;
};

#endif /* _INTERRUPT_H_ */
