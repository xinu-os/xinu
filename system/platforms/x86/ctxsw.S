/**
 * @file     ctxsw.s
 * @provides ctxsw
 *
 * $Id: ctxsw.S 2361 2011-02-22 20:13:36Z mziwisky $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

		.text
		.globl	ctxsw
newmask:	.word	0

/*------------------------------------------------------------------------
 * ctxsw -  call is ctxsw(&oldsp, &newsp, asid)
 *------------------------------------------------------------------------
 */
ctxsw:
	pushl  %ebp
	movl   %esp,%ebp

	pushfl                 /* save flags */
	pushal                 /* save general regs */

	/* save segment registers here, if multiple allowed */

	movl   8(%ebp),%eax    /* &oldsp */
	movl   %esp,(%eax)     /* save old SP */

	movl   12(%ebp),%eax   /* &newsp */
	movl   (%eax),%esp     /* restore new SP */

	/* restore new segment registers here, if multiple allowed */

	popal                  /* restore general registers */
	popfl                  /* restore flags */

	movl %ebp, %esp
	popl %ebp

	ret
