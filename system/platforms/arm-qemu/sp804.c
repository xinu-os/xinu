/**
 * @file sp804.c
 *
 * Driver for the ARM Dual Timer Module (SP804).
 *
 * This was written using the official manual from ARM Ltd.:
 *
 *  "ARM Dual Timer Module (SP804) Technical Reference Manual", revision r1p0
 *
 * This driver uses the first timer in the duo as a oneshot timer for
 * clkupdate() and the second timer in the duo as a free-running counter for
 * clkcount().
 *
 * Although Embedded Xinu could make use of the periodic timer mode that is
 * available on the SP804 to eliminate the need to call clkupdate() every timer
 * interrupt, this driver uses the oneshot timer mode to be consistent with
 * other platforms which don't have periodic timers.
 */
/* Embedded Xinu, Copyright (C) 2014.  All rights reserved. */

#include <clock.h>
#include <stdint.h>

static volatile struct {
    struct {
        uint32_t Load;     /* +0x00 */
        uint32_t Value;    /* +0x04 */
        uint32_t Control;  /* +0x08 */
        uint32_t IntClr;   /* +0x0C */
        uint32_t RIS;      /* +0x10 */
        uint32_t MIS;      /* +0x14 */
        uint32_t BGLoad;   /* +0x18 */
        uint32_t Reserved; /* +0x1C */
    } timers[2];
} * const regs = (void*)0x101E2000;
/* On the Versatile PB, there's another SP804 at 0x101E3000.  That is, another
 * dual timer, for a total of *four* timers.  But this driver doesn't use the
 * second SP804.  */

/* Flags for the timer control registers  */
#define SP804_TIMER_ENABLE       (1 << 7)
#define SP804_TIMER_PERIODIC     (1 << 6)
#define SP804_TIMER_INT_ENABLE   (1 << 5)
#define SP804_TIMER_PRESCALE_1   (0 << 2)
#define SP804_TIMER_PRESCALE_16  (1 << 2)
#define SP804_TIMER_PRESCALE_256 (2 << 2)
#define SP804_TIMER_32BIT        (1 << 1)
#define SP804_TIMER_ONESHOT      (1 << 0)

void sp804_init(void)
{
    /* Enable the second timer (free-running, no IRQ)  */
    regs->timers[1].Control = SP804_TIMER_ENABLE | SP804_TIMER_32BIT;
}

/* clkcount() interface is documented in clock.h  */
ulong clkcount(void)
{
    /* The timer counts down, hence the inversion.  */
    return ~regs->timers[1].Value;
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate(ulong cycles)
{
    /* Clear the timer IRQ if pending.  */
    regs->timers[0].IntClr = 0;

    /* Set the timer interval.  */
    regs->timers[0].Load = cycles;

    /* Enable the timer (oneshot, with IRQ)  */
    regs->timers[0].Control = SP804_TIMER_ENABLE | SP804_TIMER_32BIT |
                              SP804_TIMER_ONESHOT | SP804_TIMER_INT_ENABLE;
}
