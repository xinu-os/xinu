#include <kernel.h>
#include <stddef.h>
#include <thread.h>
#include <semaphore.h>
#include <memory.h>
#include <queue.h>
#include <interrupt.h>
#include <conf.h>

/*#define STKTRACE*/
/*#define REGDUMP*/

/*
 * The girmask is used as a mask for interrupts that don't have a
 * handler set. disable() & restore() are OR-ed with it to get the
 * mask actually used.
 */
short girmask;

extern struct idt idt[NID];
extern long defevec[];

/**
 * Set exception vector to point to an exception handler
 * @param xnum
 * @param handler pointer to handling function
 *
 */
void init_evec(void)
{
	int i;
	struct idt *pidt;

	for (i = 0; i < NID; i++)
	{
		/* No handler defined yet */
		exctab[i] = NULL;

		pidt = &idt[i];
		pidt->igd_loffset = (ushort)(defevec[i] & 0x0ffff);
		pidt->igd_hoffset = (ushort)(defevec[i] >> 16);
		pidt->igd_segsel = 0x8;		/* kernel code segment */
		pidt->igd_mbz = 0;
		pidt->igd_type = IGDT_TRAPG;
		pidt->igd_dpl = 0;
		pidt->igd_present = 1;
	}
}

void set_evec(int exc_num, int handler)
{
	struct idt *pidt;

	pidt = &idt[exc_num];
	pidt->igd_loffset = (ushort)(handler & 0x0ffff);
	pidt->igd_hoffset = (ushort)(handler >> 16);
	pidt->igd_segsel = 0x8;		/* kernel code segment */
	pidt->igd_mbz = 0;
	pidt->igd_type = IGDT_TRAPG;
	pidt->igd_dpl = 0;
	pidt->igd_present = 1;

	if ( 32 <= exc_num && exc_num < 48 ) {
		/* enable the interrupt in the global IR mask */
		exc_num -= 32;
		girmask = (short)(girmask & ~(1 << exc_num));
	}
}

void set_handler(int exc_num, interrupt (*handler)(void))
{
	exctab[exc_num] = handler;

	if ( 32 <= exc_num && exc_num < 48 ) {
		/* enable the interrupt in the global IR mask */
		exc_num -= 32;
		girmask = (short)(girmask & ~(1 << exc_num));
	}
}
