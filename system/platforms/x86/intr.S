/* intr.s - disable(), restore(), enable(), pause(), halt() */


#include <asm-i386/icu.h> 

	.globl	enable
	.globl	disable
	.globl	restore
	.globl	restore_intr
	.globl	pause
	.globl	getirmask

/*------------------------------------------------------------------------
 * _Xint? - default exception and interrupt entry points
 *	NB: These must be contiguous
 *------------------------------------------------------------------------
 */
#include <asm-i386/xint.S>

		.globl	defevec
defevec:
		.long	_Xint0x00
		.long	_Xint0x01
		.long	_Xint0x02
		.long	_Xint0x03
		.long	_Xint0x04
		.long	_Xint0x05
		.long	_Xint0x06
		.long	_Xint0x07
		.long	_Xint0x08
		.long	_Xint0x09
		.long	_Xint0x0A
		.long	_Xint0x0B
		.long	_Xint0x0C
		.long	_Xint0x0D
		.long	_Xint0x0E
		.long	_Xint0x0F
		.long	_Xint0x10
		.long	_Xint0x11
		.long	_Xint0x12
		.long	_Xint0x13
		.long	_Xint0x14
		.long	_Xint0x15
		.long	_Xint0x16
		.long	_Xint0x17
		.long	_Xint0x18
		.long	_Xint0x19
		.long	_Xint0x1A
		.long	_Xint0x1B
		.long	_Xint0x1C
		.long	_Xint0x1D
		.long	_Xint0x1E
		.long	_Xint0x1F
		.long	_Xint0x20
		.long	_Xint0x21
		.long	_Xint0x22
		.long	_Xint0x23
		.long	_Xint0x24
		.long	_Xint0x25
		.long	_Xint0x26
		.long	_Xint0x27
		.long	_Xint0x28
		.long	_Xint0x29
		.long	_Xint0x2A
		.long	_Xint0x2B
		.long	_Xint0x2C
		.long	_Xint0x2D
		.long	_Xint0x2E
		.long	_Xint0x2F

	.text

/*---------------------------------------------------------
 * enable all interrupts
 *---------------------------------------------------------
 */
enable:
	cli
	movw   girmask, %ax
	outb   %al,     $IMR1
	shrw   $8,      %ax
	outb   %al,     $IMR2
	sti
	ret


/*---------------------------------------------------------
 * disable()    - disable interrupts, save old state in ps
 *---------------------------------------------------------
 */
disable:
	cli
	pushfl
	inb    $IMR2,   %al
	shlw   $8,      %ax
	inb	   $IMR1,   %al	   /* XINU stores in logical form */
	notw   %ax             /* but APIC wants negated form */
	movw   %ax,     %dx
	movb   $0xff,   %al
	outb   %al,     $IMR2
	movb   $0xff,   %al
	outb   %al,     $IMR1
	popfl
	movw   %dx,     %ax
	ret


/*---------------------------------------------------------
 * restore(ps)    - restore interrupts to value in ps
 * STATWORD ps    (ulong ps)
 *---------------------------------------------------------
 */
restore:
	cli
	pushfl
	movl    8(%esp), %eax
	notw    %ax
	andw    girmask, %ax
	outb    %al,     $IMR1
	shrw    $8,      %ax
	outb    %al,     $IMR2
	popfl
	sti
	ret

restore_intr:
	sti
	movl    4(%esp), %eax
	call    *%eax
	ret

/*------------------------------------------------------------------------
 * getirmask(ps)  - return current interrupt mask in ps
 *------------------------------------------------------------------------
 */
getirmask:
	inb	    $IMR2,   %al
	shlw    $8,      %ax
	inb     $IMR1,   %al	
	movl    4(%esp), %edx
	movw    %ax,     (%edx)
	ret
