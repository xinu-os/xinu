/**
 * @file xtrap.c 
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <thread.h>
#include <stddef.h>
#include <stdio.h>

extern void halt(void);

#define TRAPS 16

char *trap_names[TRAPS] =
{
    "divide by zero",
    "debug exception",
    "non-maskable interrupt",
    "breakpoint",
    "overflow",
    "bounds check failure",
    "invalid opcode",
    "coprocessor not available",
    "double fault",
    "coprocessor segment overrun",
    "invalid TSS",
    "segment not present",
    "stack fault",
    "general protection violation",
    "page fault",
    "coprocessor error"
};

/**
 * Generic XINU Trap (Interrupt) Handler.
 *
 * @param cause the contents of the cause register used to decode the trap
 * @param frame pointer to the interrupt frame with saved status
 *
 */
void xtrap(long cause, long *frame)
{
    int offset = 0;

    if ( (8 == cause) || (10 <= cause && cause <= 14) )
    {
        offset = 1;
    }

    kprintf("XINU Trap/Exception 0x%02x", cause);

    if (cause < TRAPS) { kprintf(" (%s)", trap_names[cause]); }

    kprintf(": Process %d (\"%s\")\r\n", thrcurrent, thrtab[thrcurrent].name);

    kprintf(" errno: 0x%08x  ",   *(frame-2));
    kprintf("stkptr: 0x%08x\r\n", *(frame-1));
    kprintf("   edi: 0x%08x  ",   *(frame+0));
    kprintf("   esi: 0x%08x  ",   *(frame+1));
    kprintf("   ebp: 0x%08x  ",   *(frame+2));
    kprintf("   esp: 0x%08x\r\n", *(frame+3));
    kprintf("   ebx: 0x%08x  ",   *(frame+4));
    kprintf("   edx: 0x%08x  ",   *(frame+5));
    kprintf("   ecx: 0x%08x  ",   *(frame+6));
    kprintf("   eax: 0x%08x\r\n", *(frame+7));
    kprintf("   ebp: 0x%08x  ",   *(frame+8));

    kprintf("   eip: 0x%08x  ",   *(frame+offset+9));
    kprintf("    cs: 0x%08x  ",   *(frame+offset+10));
    kprintf("eflags: 0x%08x\r\n", *(frame+offset+11));

    if ( 1 == offset )
    {
        kprintf(" errno: 0x%08x\r\n", *(frame+9));
    }

    kprintf("girmask: 0x%04x\r\n", getirmask());

    halt();

    kprintf("did not halt.");
    while (1)
    {
    }
}
