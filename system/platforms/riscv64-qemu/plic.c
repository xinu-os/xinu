/**
 * @file pl190.c
 *
 * Driver for RISCV Platform-Level Interrupt Controller (PLIC)
 * (see the RISC-V privileged architecture spec)
 *
 */
/* Embedded Xinu, Copyright (C) 2014.  All rights reserved. */

#include <riscv.h>
#include <interrupt.h>
#include <stdint.h>
#include <thread.h>

extern int resdefer;
void* plic_base = NULL;

// riscv PLIC supports 128 interrupts starting from 1,
// IRQ ID 0 is unused, so we'll use irq 0 for handling timer interrupts
interrupt_handler_t interruptVector[129];

/* offset valus on basis of the SiFive U54 Core docs */
#define PLIC_IRQ_PRI          (plic_base)
#define PLIC_IRQ_ENABLE       (plic_base + 0x002000L)
#define PLIC_IRQ_THRESHOLD    (plic_base + 0x200000L)
#define PLIC_IRQ_CLAIM        (plic_base + 0x200004L)
#define PLIC_IRQ_PENDING      (plic_base + 0x001000L)

void plic_enable_irq(int num) {
    *(((volatile uint32_t*)PLIC_IRQ_ENABLE)) |= (0x1 << num);
}

void plic_disable_irq(int num) {
    *(((volatile uint32_t*)PLIC_IRQ_ENABLE)) &= ~(0x1 << num);
}

void plic_init(void) {
    int i;
    // Clear interrupt pending and interrupt enable bits.  The first
    // bit of the first word is unused (non existing irq 0), just one
    // bit is used in the last word for irq 128
    for (i = 0; i < 5; i++) {
	*((volatile uint32_t*)(PLIC_IRQ_ENABLE + (i << 2))) = 0;
	*((volatile uint32_t*)(PLIC_IRQ_PENDING + (i << 2))) = 0;
    }
    // priority registers are 32 bit
    for (i = 1; i <= 128; i++) {
	// highest priority for all
	*((volatile uint32_t*)(PLIC_IRQ_PRI  + i)) = 0x07;
    }
    // interrupts with priority > than threshold will trigger
    *((volatile uint32_t*)PLIC_IRQ_THRESHOLD)=0x0;
    // enable external interrupts
    set_csr(mie, MIP_MEIP);
}


/**
 * Enable an interrupt request line.
 *
 * @param irq
 *      Number of the IRQ to enable, which on this platform must be in the
 *      range [0, 31].
 * 
 * PLIC does not really have  IRQ0 (IRQ ID 0 means no interrupt), so
 * in XINU we logically map IRQ number to RISCV timer interrupt, which
 * is handled in  CLINT
 */
void enable_irq(irqmask irq)
{
  if (irq) {
      plic_enable_irq(irq);
  } else {
      //enable timer interrupt
      set_csr(mie, MIP_MTIP);
  }
}

/**
 * Disable an interrupt request line.
 *
 * @param irq
 *      Number of the IRQ to disable, which on this platform must be in the
 *      range [0, 31].
 */
void disable_irq(irqmask irq)
{
  if (irq) {
      plic_disable_irq(irq);
  } else {
      //disable timer interrupt
      clear_csr(mie, MIP_MTIP);
  }
}


void dispatch_timer(void)
{
    interruptVector[0]();
}

/**
 * Call the service routine for each pending IRQ, after this 
 * reschedule if needed.
 */
void dispatch(void) {
    int irq;
    // get pending irq of maximum priority
    while ((irq = *(((volatile uint32_t*)PLIC_IRQ_CLAIM)))) {
	// handle it
	interruptVector[irq]();
	// clear claim
	*(((volatile uint32_t*)PLIC_IRQ_CLAIM)) = irq;
    }
    if (--resdefer > 0)
    {
        resdefer = 0;
	resched();
    }
}

