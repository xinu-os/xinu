/**
 * @file     timer.c
 *
 * This file contains C code that interfaces with the BCM2835 system timer on
 * the Raspberry Pi.
 *
 * See 12. "System Timer" of the BCM2835-ARM-Peripherals.pdf document for
 * details about this hardware.  However, this document does not say the
 * frequency the timer runs at (it's 1 MHz), nor does it say that some of the
 * output compare registers (0 and 2) are used by the GPU and therefore cannot
 * be used by ARM code, nor does it say (correctly) what interrupt line the
 * timer is connected to (it's interrupt line 3, at least for the index 3 output
 * compare register).
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <clock.h>
#include <interrupt.h> /* for memory_barrier() */

/** Layout of the BCM2835 System Timer's registers.  */
struct bcm2835_timer_regs {
    uint CS;  /** System Timer Control/Status */
    uint CLO; /** System Timer Counter Lower 32 bits */
    uint CHI; /** System Timer Counter Higher 32 bits */
    uint C0;  /** System Timer Compare 0.  DO NOT USE; is used by GPU.  */
    uint C1;  /** System Timer Compare 1 */
    uint C2;  /** System Timer Compare 2.  DO NOT USE; is used by GPU.  */
    uint C3;  /** System Timer Compare 3 */
};

#define BCM2835_SYSTEM_TIMER_BASE 0x20003000

#define BCM2835_SYSTEM_TIMER_MATCH_0 (1 << 0)
#define BCM2835_SYSTEM_TIMER_MATCH_1 (1 << 1)
#define BCM2835_SYSTEM_TIMER_MATCH_2 (1 << 2)
#define BCM2835_SYSTEM_TIMER_MATCH_3 (1 << 3)

static volatile struct bcm2835_timer_regs * const regs =
        (volatile struct bcm2835_timer_regs*)BCM2835_SYSTEM_TIMER_BASE;

/* clkcount() interface is documented in clock.h  */
/**
 * @detail
 *
 * Raspberry-Pi specific note:  This function returns the low-order 32 bits of
 * the BCM2835's free-running counter.  This counter runs at 1 MHz and therefore
 * overflows every 4295 seconds.
 */
ulong clkcount(void)
{
    ulong count;

    memory_barrier();
    count = regs->CLO;
    memory_barrier();
    return count;
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate(ulong cycles)
{
    memory_barrier();

    /* To set the timer, we are using C3 (System Timer Compare 3), which is one
     * of the System Timer's output compare registers not being used by the GPU.
     * When the low bits (CLO) of the timer match C3, the hardware will generate
     * a timer interrupt.  */

    /* First clear the current interrupt, if any, by clearing the
     * BCM2835_SYSTEM_TIMER_MATCH_3 bit in the timer control/status register.
     * Unintuitively, according to the Broadcom documentation this must be done
     * by _writing_ a 1 to the bit to be cleared. */
    regs->CS = BCM2835_SYSTEM_TIMER_MATCH_3;

    /* Next, set up the new value in the C3 output compare register.  This is
     * done by reading the current low-order bits of the counter and adding the
     * requested number of cycles.  Note that wraparounds will necessarily be
     * handled correctly because the output compare registers are by design only
     * 32 bits.  */
    regs->C3 = regs->CLO + cycles;

    memory_barrier();
}
