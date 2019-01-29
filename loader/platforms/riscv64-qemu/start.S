/**
 * @file start.S
 *
 * Initialization code for Embedded Xinu on RISCV-QEMU virt board
 *	
 */
	
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */


#include <riscv.h>    

// sizes for fixed stacks	
#define NULLSTK   8192
#define PANICSTK  8192


.section .init
	.globl _start

	/* _start:  Entry point of the Xinu kernel */
	.func _start
_start:
	j reset_handler
	.endfunc

.align 0x4
        /* vector of interrupt and exception handlers, each entry is 4
	bytes, so it can contain just one uncompressed or two
	compressed instructions */
_vectors:
	/* 0: User software interrupt/Exception */
	c.j exception_handler
	c.nop
	/* 1: Supervisor software interrupt */
	c.j exception_handler
	c.nop
	/* 2: */
	c.j exception_handler
	c.nop
	/* 3: Machine software interrupt */
	c.j exception_handler
	c.nop	
	/* 4 User timer interrupt */
	c.j exception_handler
	c.nop
	/* 5 Supervisor timer interrupt */
	c.j exception_handler
	c.nop	
	/* 6 */
	c.j exception_handler
	c.nop
	/* 7 Machine timer interrupt */
	c.j mach_timer_int
	c.nop
	/* 8 User external interrupt */
	c.j exception_handler
	c.nop
	/* 9 Supervisor external interrupt */
	c.j exception_handler
	c.nop
	/* 10 Reserved */
	c.j exception_handler
	c.nop
	/* 11 Machine external interrupt */
	c.j mach_ext_int
	c.nop
	/* 12 - 15 reserved*/
	c.j exception_handler
	c.nop
	c.j exception_handler
	c.nop
	c.j exception_handler
	c.nop
	c.j exception_handler
	c.nop

.macro SAVEREGS
	addi sp,sp,-RISCV_CONTEXT_SIZE 
	sd x1, 1*REGBYTES(sp)
	sd x2, 2*REGBYTES(sp)	
	sd x3, 3*REGBYTES(sp)
	sd x4, 4*REGBYTES(sp)
	sd x5, 5*REGBYTES(sp)
	sd x6, 6*REGBYTES(sp)	
	sd x7, 7*REGBYTES(sp)
	sd x8, 8*REGBYTES(sp)	
	sd x9, 9*REGBYTES(sp)	
	sd x10, 10*REGBYTES(sp)	
	sd x11, 11*REGBYTES(sp)	
	sd x12, 12*REGBYTES(sp)	
	sd x13, 13*REGBYTES(sp)	
	sd x14, 14*REGBYTES(sp)	
	sd x15, 15*REGBYTES(sp)	
	sd x16, 16*REGBYTES(sp)	
	sd x17, 17*REGBYTES(sp)	
	sd x18, 18*REGBYTES(sp)	
	sd x19, 19*REGBYTES(sp)	
	sd x20, 20*REGBYTES(sp)	
	sd x21, 21*REGBYTES(sp)	
	sd x22, 22*REGBYTES(sp)	
	sd x23, 23*REGBYTES(sp)	
	sd x24, 24*REGBYTES(sp)	
	sd x25, 25*REGBYTES(sp)	
	sd x26, 26*REGBYTES(sp)	
	sd x27, 27*REGBYTES(sp)	
	sd x28, 28*REGBYTES(sp)	
	sd x29, 29*REGBYTES(sp)	
	sd x30, 30*REGBYTES(sp)	
	sd x31, 31*REGBYTES(sp)

	csrr t0, mepc
	sd t0, 0(sp)
.endm

.macro RESTOREREGS
	ld t0, 0(sp)
	csrw mepc,t0

	ld x1, 1*REGBYTES(sp)
	ld x2, 2*REGBYTES(sp)	
	ld x3, 3*REGBYTES(sp)
	ld x4, 4*REGBYTES(sp)
	ld x5, 5*REGBYTES(sp)
	ld x6, 6*REGBYTES(sp)	
	ld x7, 7*REGBYTES(sp)
	ld x8, 8*REGBYTES(sp)	
	ld x9, 9*REGBYTES(sp)	
	ld x10, 10*REGBYTES(sp)	
	ld x11, 11*REGBYTES(sp)	
	ld x12, 12*REGBYTES(sp)	
	ld x13, 13*REGBYTES(sp)	
	ld x14, 14*REGBYTES(sp)	
	ld x15, 15*REGBYTES(sp)	
	ld x16, 16*REGBYTES(sp)	
	ld x17, 17*REGBYTES(sp)	
	ld x18, 18*REGBYTES(sp)	
	ld x19, 19*REGBYTES(sp)	
	ld x20, 20*REGBYTES(sp)	
	ld x21, 21*REGBYTES(sp)	
	ld x22, 22*REGBYTES(sp)	
	ld x23, 23*REGBYTES(sp)	
	ld x24, 24*REGBYTES(sp)	
	ld x25, 25*REGBYTES(sp)	
	ld x26, 26*REGBYTES(sp)	
	ld x27, 27*REGBYTES(sp)	
	ld x28, 28*REGBYTES(sp)	
	ld x29, 29*REGBYTES(sp)	
	ld x30, 30*REGBYTES(sp)	
	ld x31, 31*REGBYTES(sp)	
	addi sp,sp,RISCV_CONTEXT_SIZE
.endm	
	
	/* external interrupt handler */
mach_ext_int:
	SAVEREGS		
	jal dispatch
	// make mret enable interrupts and stay in  Machine Mode
	// (we may get here when process voluntary calls resched,
	//  in this case MPP will be 0 (because no interrupt occured
	//  and MPIE will be 1 (because resched() runs with interrupts
	//  disbled
	li t0, MSTATUS_MPP | MSTATUS_MPIE
	csrs mstatus, t0
	RESTOREREGS
	mret

mach_timer_int:
	SAVEREGS			
	jal dispatch_timer
	// see comment above in mach_ext_int
	li t0, MSTATUS_MPP | MSTATUS_MPIE
	csrs mstatus, t0
	RESTOREREGS
	mret
	
exception_handler:
	// pass orig. sp value as parameter
	addi a0, sp, 0
	// set sp to panic stack
	la sp, panicstk
	ld sp, 0(sp)
	// jump to the C routine and stay there
	j handle_exception
	
.section .text
	/* reset_handler: Reset handler routine executed to start up the kernel
	 * when the CPU processor is reset */
	.func reset_handler
reset_handler:
	// disable all interrupts, clear pending interrupts
	li a0, MSTATUS_MIE | MSTATUS_SIE | MSTATUS_MPIE | MSTATUS_SPIE
	csrc mstatus, a0
	//csrw mstatus, x0
	csrw mie, x0
	csrw mip, x0	

	/* fill cpuid from misa:
	 * misa is not exactly the unique cpu id (the (marchid, mvendorid) tuple is)
	 * but it gives us isa variant in the bits [62,63]
	 * and extensions list in the lowest 26 bits,
	 */
	csrr t1, misa
	la   t0, cpuid
	sd t1, 0(t0)
	
	// save dtb arrdess from a1
	la   t0, dtb_addr
	sd   a1, 0(t0)

	/* interrupt vector setup */
	/* write mtvec */
	la t0, _vectors
	// lower bit configures vectored interrupt mode
	ori t0, t0, 1
	csrw mtvec, t0
check_mtvec:
	/* Make sure it sticks: why do we need this? spike bug? */
	csrr t1, mtvec
        bne t0, t1, check_mtvec
	
	la sp, _end
	/* Put the panick stack (used by exception handlers that 
	 * prints the panic message with register dump)
	 * directly after end of bss */
	li t1, PANICSTK
	add sp, sp, t1
	la t0, panicstk
	sd sp, 0(t0)
	
	/* Put the null thread's stack directly after the panic stack.  */	
	li t1, NULLSTK
	add sp, sp, t1
	la t0, memheap
	sd sp, 0(t0)
	/* jump to the initialization routine */

	j nulluser

	.endfunc

