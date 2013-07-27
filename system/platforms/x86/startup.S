/*
 * Startup code for standalone system
 * Non-relocating version -- for programs which are loaded by boot
 * Relocating version for boot
 * Small relocating version for "micro" boot
 */

#define NULLMAGIC 0x0A0AAAA9

	.data

	.globl	gdt
	.globl	gdtr
	.extern memheap
	.extern initialsegs
	.extern ownerTable

	/* Intel enjoys the GDT to be 16 bit aligned */
	.align 16
gdt:	.space 64     # must be NSEGS * 8 (number of bytes in table)
	.align 4
gdtr:	.word  63  # set the gdt limit
     	.long  gdt # gdt base, will be filled later

	.globl	idt
	.globl	idtr
idt:	.space	384	# must equal NID*8 (384 == 48 vectors)
idtr:	.word	383	# size of idt - 1 (in bytes)
		.long	idt

	.globl cpudelay

cpudelay:	.long	1
	.text
	.align 2
	.extern nulluser
	.globl _bootdev
	.globl _cyloffset
	.globl lidt
	.globl startup

startup:
	/* clear stack */
	movl	%esp, %ecx
	movl	$_end, %eax
	subl	%eax, %ecx
	subl	$5*4, %ecx
	pushl	%ecx
	pushl	$_end
	call	_asm_bzero
	addl	$2*4, %esp

	pushl	$NULLMAGIC  # put the magic number in place

	/*
	 * Clear Bss section
	 */
	movl	$_end,  %ecx  # Clear from `edata` to `_end`
	subl	$edata, %ecx
	pushl	%ecx
	pushl	$edata
	call	_asm_bzero
	addl    $2*4, %esp

	/*
	 * Clear all flags
	 */
	pushl	$0
	popf

	/* place the UART in a sane state               */
	movl $0x3f8+3, %edx /* UART_LCR                 */
	movl $0x80,    %eax /* UART_LCR_DLAB            */
	outb %al,      %dx

	movl $0x3f8,   %edx   /* UART_DLL               */
	movl $0x01,    %eax   /* DLL = 1 for 115200 bps */
	outb %al,      %dx

	movl $0x3f8+1, %edx   /* UART_DLM */
	movl $0x0,     %eax   /* DLM = 0 */
	outb %al,      %dx

	movl $0x3f8+3, %edx /* UART_LCR */
	movl $0x03,    %eax /* UART_LCR_8N1 */
	outb %al,      %dx

	call	initialsegs

	# save the current stack pointer
	movl	%esp, %esi

	lgdt	gdtr           # load the table into the system

    # JSLinux assumes O/S handles protection, so this won't work
#	movw	$1, %ax # turn on protected mode bit
#	lmsw	%ax     # bam, we're running in protected mode


	/****************************/
	/* reload segment registers */
	/****************************/

	/* 
	 * jump to code using the gdt located at the byte 8 for the gdtr 
	 * and located at the address of $gdt1
	 */
	ljmp	$0x08, $gdt1	/* CS descriptor 1 */
gdt1:
	/*
	 * load gdt starting at byte 16 into the data segments (ds/es) and the
	 * same (since we are using flat memory space) into the stack segment.
	 */
	movl	$0x10, %eax	/* DS descriptor 2 */
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %ss

	movl    $0x18, %eax /* TLS descriptor 3 */
	movw    %ax, %gs

	# Restore the stack point post-segment init
	movl	%esi, %esp

	# find where the stack starts and mark the beginning of heap mem
	movl	%esp, %ebx
heapat:
	movl	0(%ebx), %eax
	addl	$4, %ebx
	testl	$NULLMAGIC, %eax
	jne		heapat

	addl	$4, %ebx
	leal	memheap, %edx
	movl	%ebx, 0(%edx)

	/* set monitor coprocessor bit in CR0 */
	movl %CR0, %eax
	orl  $0x0002, %eax
	movl %eax, %CR0

	/* set O/S extended exception support and restore bits in CR4 */
	movl %CR4, %eax
	orl  $0x0600, %eax
	movl %eax, %CR4

	/* stack pointer is base pointer (prepair for C code) */
	movl	%esp, %ebp  /* preparing for C code           */

	finit /* <SIGH> Initialize the floating point unit */

	movl    $0x01, %eax
	cpuid
	pushl	%esp
	pushl   %eax
	call	nulluser    /* C code, go!                    */

	jmp halt

	.data
_bootdev:	.long	0
_cyloffset:	.long	0
savearea:	.long	0,0	# sp & bp to return to
	.text

	.globl	inb
inb:
	movl 4(%esp), %edx
	xorl %eax, %eax
	inb  %dx, %al
	ret

	.globl	inw
inw:
	movl 4(%esp), %edx
	xorl %eax, %eax
	inw  %dx, %ax
	ret

	.globl	outb
outb:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outb %al, %dx
	ret

	.globl	outw
outw:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outw %ax, %dx
	ret

	#
	# _asm_bzero (base, count)
	#
	.globl _asm_bzero
_asm_bzero:
	pushl	%edi
	movl	8(%esp),%edi
	movl	12(%esp),%ecx
	movb	$0x00,%al
	cld
	rep
	stosb
	popl	%edi
	ret

	#
	# bcopy(src, dst, count)
	#

	.globl	_asm_bcopy
_asm_bcopy:
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx
	cld
	rep
	movsb
	popl	%edi
	popl	%esi
	ret

	# insw(port,addr,cnt)
	.globl	insw
insw:
	pushl	%edi
	movw	8(%esp),%dx
	movl	12(%esp),%edi
	movl	16(%esp),%ecx
	cld
	.byte 0x66,0xf2,0x6d	# rep insw
	movl	%edi,%eax
	popl	%edi
	ret

	# outsw(port,addr,cnt)
	.globl	outsw
outsw:
	pushl	%esi
	movw	8(%esp),%dx
	movl	12(%esp),%esi
	movl	16(%esp),%ecx
	cld
	.byte 0x66,0xf2,0x6f	# rep outsw
	movl	%esi,%eax
	popl	%esi
	ret

	#
	# lidt() - load interrupt descriptor table from idtr
	#
lidt:
	lidt	idtr
	ret

#Return:
#	CF clear if successful
#	AX = extended memory between 1M and 16M, in K (max 3C00h = 15MB)
#	BX = extended memory above 16M, in 64K blocks
#	CX = configured memory 1M to 16M, in K
#	DX = configured memory above 16M, in 64K blocks CF set on error
