/**
 * @file dispatch.c
 *
 * This file contains the code to interact with the interrupt controller
 * hardware on the BCM2835 (Raspberry Pi).  This poorly-documented interrupt
 * controller allows for 64 interrupt lines allegedly shared between the ARM and
 * GPU, as well as a certain number of additional interrupt lines for the ARM,
 * which our code here maps to IRQ indices 64 and greater.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */


#include <interrupt.h>
#include <stddef.h>
#include <kernel.h> /* for kprintf() */

extern void halt(void);

/** Layout of the BCM2835 interrupt controller's registers. */
struct bcm2835_interrupt_registers {
    uint IRQ_basic_pending;
    uint IRQ_pending_1;
    uint IRQ_pending_2;
    uint FIQ_control;
    uint Enable_IRQs_1;
    uint Enable_IRQs_2;
    uint Enable_Basic_IRQs;
    uint Disable_IRQs_1;
    uint Disable_IRQs_2;
    uint Disable_Basic_IRQs;
};

#define BCM2835_INTERRUPT_REGS_BASE 0x2000B200

static volatile struct bcm2835_interrupt_registers * const regs =
        (volatile struct bcm2835_interrupt_registers*)BCM2835_INTERRUPT_REGS_BASE;

/* Number of IRQs shared between the GPU and ARM. These correspond to the IRQs
 * that show up in the IRQ_pending_1 and IRQ_pending_2 registers.  */
#define BCM2835_NUM_GPU_SHARED_IRQS     64

/* Number of ARM-specific IRQs. These correspond to IRQs that show up in the
 * first 8 bits of IRQ_basic_pending.  */
#define BCM2835_NUM_ARM_SPECIFIC_IRQS   8

/* Total number of IRQs on this hardware.  */
#define BCM2835_NUM_IRQS (BCM2835_NUM_GPU_SHARED_IRQS + BCM2835_NUM_ARM_SPECIFIC_IRQS)

/** Table of Xinu's interrupt handler functions.  This is an array mapping IRQ
 * numbers to handler functions.  */
interrupt_handler_t interruptVector[BCM2835_NUM_IRQS];

/** Bitwise table of IRQs that have been enabled on the ARM. */
static uint arm_enabled_irqs[3];

/* Call the handler function for an IRQ that was received, or panic if it
 * doesn't exist.  */
static void handle_irq(uchar irq_num)
{
    interrupt_handler_t handler = interruptVector[irq_num];
    if (handler)
    {
        (*handler)();
    }
    else
    {
        kprintf("ERROR: No handler registered for interrupt %u\r\n", irq_num);
        halt();
    }
}

/* Check if the pending bit for an IRQ line is set; if so, call the handler
 * function.  */
static void check_irq_pending(uchar irq_num)
{
    bool handle = FALSE;

    /* Check the appropriate hardware register, depending on the IRQ number.  */
    if (irq_num >= 64)
    {
        if (regs->IRQ_basic_pending & (1 << (irq_num - 64)))
        {
            handle = TRUE;
        }
    }
    else if (irq_num >= 32)
    {
        if (regs->IRQ_pending_2 & (1 << (irq_num - 32)))
        {
            handle = TRUE;
        }
    }
    else
    {
        if (regs->IRQ_pending_1 & (1 << irq_num))
        {
            handle = TRUE;
        }
    }
    if (handle)
    {
        handle_irq(irq_num);
        /* The pending bit should have been cleared in a device-specific way by
         * the handler function.  As far as we can tell, it cannot be cleared
         * directly through the interrupt controller.  */
    }
}

/* Find index of first set bit in a nonzero word. */
static inline ulong first_set_bit(ulong word)
{
    return 31 - __builtin_clz(word);
}

/**
 * Processes all pending interrupt requests.
 *
 * On the BCM2835 (Raspberry Pi), this done by iterating through all registered
 * interrupts on the ARM and checking whether each one is pending.  This is not
 * necessarily the fastest way to do it, but this should minimize problems with
 * the poorly-documented hardware and conflicts with the GPU.
 */
void dispatch(void)
{
    uint i;

    for (i = 0; i < 3; i++)
    {
        uint mask = arm_enabled_irqs[i];
        while (mask != 0)
        {
            uint bit = first_set_bit(mask);
            mask ^= (1 << bit);
            check_irq_pending(bit + (i << 5));
        }
    }
}

/**
 * Enable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to enable, which must be valid on the current
 *      platform.
 */
void enable_irq(irqmask irq_num)
{
    if (irq_num < 32)
    {
        regs->Enable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] |= 1 << irq_num;
    }
    else if (irq_num < 64)
    {
        regs->Enable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] |= 1 << (irq_num - 32);
    }
    else
    {
        regs->Enable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] |= 1 << (irq_num - 64);
    }
}

/**
 * Disable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to disable, which must be valid on the current
 *      platform.
 */
void disable_irq(irqmask irq_num)
{
    if (irq_num < 32)
    {
        regs->Disable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] &= ~(1 << irq_num);
    }
    else if (irq_num < 64)
    {
        regs->Disable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] &= ~(1 << (irq_num - 32));
    }
    else
    {
        regs->Disable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] &= ~(1 << (irq_num - 64));
    }
}
