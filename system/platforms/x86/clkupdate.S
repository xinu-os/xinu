/* clockupdate.s - clock_update */

#include <asm-i386/icu.h>

.text
	.globl	clkupdate
clkupdate:
	movb   $EOI, %al   /* write end-of-interrupt signal */
	outb   %al,  $OCR1 /*     to interrupt control unit */
	ret


	.globl clockIRQ
clockIRQ:
	cli
	pushal
	call clkhandler
	popal
	sti
	iret
