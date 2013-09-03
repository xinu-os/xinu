#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <testsuite.h>

#define TIMES 5

/* count number of calls to t5 (also no compiler optimization) */
static int calls = 0;

static void t5(int times, uchar *testArray, int *index)
{
    /* Disable interrupts to prevent race conditions with the index update. */
    disable();
    if (times > 0)
    {
        testArray[*index] = thrcurrent;
        *index = *index + 1;
        testArray[*index] = times;
        *index = *index + 1;
        yield();
        t5(times - 1, testArray, index);
        calls++;
    }
}

thread test_recursion(bool verbose)
{
    tid_typ atid, btid, ctid, dtid, i, j;
    bool passed = TRUE;
    uchar testArray[4 * 2 * TIMES];
    uchar expectedResults[4 * 2 * TIMES];
    char str[50];
    int index = 0;
    int btid_remaining, dtid_remaining;

    /* reset the number of calls to `t5` */
    calls = 0;

    /* disable interrupts to prevent a preemption from launching a 
     * lower-priority thread before the higher-priority ones are created */
    disable();

    ready(atid = create((void *)t5, INITSTK, 31, "RECURSION-A",
                        3, TIMES, testArray, &index), 0);
    ready(btid = create((void *)t5, INITSTK, 32, "RECURSION-B",
                        3, TIMES, testArray, &index), 0);
    ready(ctid = create((void *)t5, INITSTK, 34, "RECURSION-C",
                        3, TIMES, testArray, &index), 0);
    ready(dtid = create((void *)t5, INITSTK, 32, "RECURSION-D",
                        3, TIMES, testArray, &index), 0);

    /* Run the tests by yielding the processor */
    yield();

    /* Generate expected results and compare */

    for (i = 0, j = TIMES; i < 2 * TIMES; i += 2, j--)
    {
        expectedResults[i] = ctid;
        expectedResults[i + 1] = j;
    }

    /* Since threads may be rescheduled before they can disable interrupts, the
     * exact order than threads B and D, which have equal priority, execute in
     * is unpredictable.  To avoid spurious test failures, we accept them
     * executing in any order.  */
    btid_remaining = TIMES;
    dtid_remaining = TIMES;
    for (i = TIMES * 2; i < 6 * TIMES; i += 2)
    {
        if (testArray[i] == btid && btid_remaining > 0) {
            expectedResults[i] = btid;
            expectedResults[i + 1] = btid_remaining;
            btid_remaining--;
        } else {
            expectedResults[i] = dtid;
            expectedResults[i + 1] = dtid_remaining;
            dtid_remaining--;
        }
    }


    for (i = 6 * TIMES, j = TIMES; i < 8 * TIMES; i += 2, j--)
    {
        expectedResults[i] = atid;
        expectedResults[i + 1] = j;
    }

    for (i = 0; i < 8 * TIMES; i++)
    {
        if (expectedResults[i] != testArray[i])
        {
            passed = FALSE;
            sprintf(str, "Expected testArray[%d] == %d, not %d\n",
                    i, expectedResults[i], testArray[i]);
            testFail(verbose, str);
        }
    }

    /* t5 should be called (4 threads * NTIMES) = 20 times */
    testPrint(verbose, "Checking number of recursive calls");
    failif(calls != 20, "called `t5` excessivly");

    if (TRUE == passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
