/**
 * @file mailbox.c
 *
 * Provides communication channels between VC and ARM.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stddef.h>
#include <framebuffer.h>
#include <bcm2835.h>

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
    ulong n;

	pre_peripheral_read_mb();
	n = *(volatile ulong *)(MMIO_BASE + base + reg);
    post_peripheral_read_mb();
    return n;
}

/* The opposite of above. Write to MMIO. */
void writeMMIO(ulong base, ulong reg, ulong val)
{
	pre_peripheral_write_mb();
	*(volatile ulong *)(MMIO_BASE + base + reg) = val;
	post_peripheral_write_mb();
}


/* Converts ARM physical addresses to ARM bus addresses.
 * Separate function for legibility's sake.
 * Rev 1 board GPU required bus addresses. Now deprecated. */
ulong physToBus(void *address) {
	return ((ulong)address) + 0xC0000000;
}
