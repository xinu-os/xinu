#include <stdio.h>
#include <stddef.h>
#include <testsuite.h>
#include <interrupt.h>
#include <thread.h>

thread spin(void)
{
#ifdef MIPS
    int i;
    for (i = IRQ_HW0; i <= IRQ_HW5; i++)
    {
        disable_irq(i);
    }
    enable_irq(IRQ_TIMER);
    enable();

    while (TRUE)
        ;

    return OK;
#else
    return SYSERR;
#endif
}

/**
 * Example of a test program for the Xinu testsuite.  Beyond this file you
 * must add an entry to the testtab in testhelper.c and a prototype in
 * testsuite.h.
 */
thread test_preempt(bool verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;
    tid_typ thrspin;

    /* This is the first "subtest" of this suite */
    thrspin =
        create(spin, INITSTK, thrtab[thrcurrent].prio, "test_spin", 0);

    /* Make spin ... spin */
    ready(thrspin, RESCHED_YES);

    /* If this next line runs, we're good */
    kill(thrspin);

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
