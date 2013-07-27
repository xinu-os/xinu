#include <kernel.h>
#include <interrupt.h>
#include <segment.h>

extern void clockintr(void);
extern void xtrap(int, int *);

interrupt (*exctab[NID])(void);

void dispatch(int exc_num, int *stack_ptr)
{
	int irq_num = exc_num-IRQBASE;
	if ( exctab[exc_num] != NULL )
	{
		/* execute handler */
		(*exctab[exc_num])();
	}
	else
	{
		xtrap(exc_num, stack_ptr);
	}

	return;
}
