#include <asm-i386/icu.h>

.data
PARPORT: .string "Parallel Port: 0x%08x\r\n"

.text
	.globl parport
parport:
	cli
	pushal

	/* send end-of-interrupt signal */
	movb  $EOI, %al
	outb  %al, $OCR1

	popal
	sti
	iret
