/**
 * @file kexec.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <interrupt.h>
#include <kernel.h>
#include <kexec.h>
#include <string.h>


/* The below array contains a stub of ARM instructions used to copy the
 * new kernel into its final location, then pass control to it.
 *
 * This stub is hard-coded as an array because it needs to be copied to a
 * location in which it cannot be overwritten by itself while copying the new
 * kernel.  Therefore, its size needs to be known and it needs to be fully
 * relocatable (which in theory the assembler does not guarantee).
 *
 * Arguments are:
 *
 * r0:  pointer to new kernel
 * r1:  size of new kernel
 * r2:  pointer to ARM boot tags (preserved in r2 for convenience of new kernel)
 *
 * This is hard-coded to copy the kernel to address 0x8000.
 */

/*00000000 <copy_kernel>:*/
   /*0:	e3a04902 	mov	r4, #32768	; 0x8000*/
   /*4:	e4903004 	ldr	r3, [r0], #4*/
   /*8:	e4843004 	str	r3, [r4], #4*/
   /*c:	e2511001 	subs	r1, r1, #1*/
  /*10:	1afffffb 	bne	4 <copy_kernel+0x4>*/
  /*14:	e3a0f902 	mov	pc, #32768	; 0x8000*/
static const ulong copy_kernel[] = {
    0xe3a04902,
    0xe4903004,
    0xe4843004,
    0xe2511001,
    0x1afffffb,
    0xe3a0f902,
};

#define COPY_KERNEL_ADDR ((void*)(0x8000 - sizeof(copy_kernel)))

/**
 * Kernel execute - Transfer control to a new kernel.
 *
 * This is the Raspberry Pi implementation.  In this implementation, the new
 * kernel must be valid for the Raspberry Pi, including being linked to run at
 * and having an entry point at address 0x8000.
 *
 * @param kernel
 *      Pointer to the new kernel image loaded anywhere in memory.
 * @param size
 *      Size of the new kernel image.
 *
 * @return
 *      This function never returns.  If it somehow does, then something has
 *      gone horribly wrong.
 */
syscall kexec(const void *kernel, uint size)
{
    irqmask im;

    im = disable();

    /* Copy the assembly stub into a safe location.  */
    memcpy(COPY_KERNEL_ADDR, copy_kernel, sizeof(copy_kernel));

    /* Enter the assembly stub to copy the new kernel into its final location,
     * then pass control to it.  */
    extern void *atags_ptr;
    (( void (*)(const void *, ulong, void *))(COPY_KERNEL_ADDR))
                (kernel, size, atags_ptr);

    /* Control should never reach here.  */
    restore(im);
    return SYSERR;
}
