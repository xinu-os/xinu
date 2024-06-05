/**
 * @file riscvclk.c
 *
 * Driver for the RISCV RTC timer. 
 *
 * The hardware interface is documented in the RISCV privileged
 * architecture specification.  It looks that QEMU implementation is
 * based on the SiFive: the CLINT offsets match those the SiFive U54
 * Core Complex Manual
 *
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <clock.h>
#include <stdint.h>
#include <riscv.h>

volatile uint64_t* mtime;
volatile uint64_t* mtimecmp;

/* clkcount() interface is documented in clock.h  */
ulong clkcount(void)
{
    return *mtime;
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate(ulong cycles)
{
    *mtimecmp = (*mtime) + cycles;
}
