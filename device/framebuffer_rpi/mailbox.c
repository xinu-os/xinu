/**
 * @file mailbox.c
 *
 * Provides communication channels between VC and ARM.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stddef.h>
#include <framebuffer.h>
#include <kernel.h> /* for kprintf() */
#include <interrupt.h> //for memory_barrier

/* Read from mailbox one on channel one (GPU mailbox) */
/* Note: Data: first 28 bits. Channel: last 4 bits.   */
ulong mailboxRead() {
	ulong result = 0;
	while ((result & 0xF) != MAILBOX_CHANNEL) {
		while (readMMIO(MAILBOX_BASE, MAILBOX_STATUS) & MAILBOX_EMPTY)
			; //wait for data to arrive
		result = readMMIO(MAILBOX_BASE, MAILBOX_READ);
	}
	return result & 0xFFFFFFF0; //account for offset
}

/* Write to GPU mailbox. */
void mailboxWrite(ulong data) {
	ulong toWrite = MAILBOX_CHANNEL | (data & 0xFFFFFFF0);
	while (readMMIO(MAILBOX_BASE, MAILBOX_STATUS) & MAILBOX_FULL)
		; //wait for space
	writeMMIO(MAILBOX_BASE, MAILBOX_WRITE, toWrite);

}

/* To omit illegible lines of code, a helper function that reads from
 * memory mapped IO registers. */
ulong readMMIO(ulong base, ulong reg)
{
	memory_barrier();
	return *(volatile ulong *)(MMIO_BASE + base + reg);
}

/* The opposite of above. Write to MMIO. */
void writeMMIO(ulong base, ulong reg, ulong val)
{
	memory_barrier();
	*(volatile ulong *)(MMIO_BASE + base + reg) = val;
	memory_barrier();
}


/* Converts ARM physical addresses to ARM bus addresses.
 * Separate function for legibility's sake.
 * Rev 1 board GPU required bus addresses. Now deprecated. */
ulong physToBus(void *address) {
	return ((ulong)address) + 0xC0000000;
}
