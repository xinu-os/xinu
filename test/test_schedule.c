#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <testsuite.h>

#define TIMES 5

static void t4(int times, uchar *testArray, int *shared)
{
    int i = 0;

    /* Disable interrupts so we execute in a known order.
     * NOTE: even when doing this, this test is still subject to a race
     * condition where the running thread may be replaced with an equal-priority
     * thread before reaching the call to disable() below.  As a result, it is
     * unpredictable whether thread B or thread D will write to the array first.
     * But regardless of which one does, they should alternate back and forth.
     * */
    disable();

    for (i = 0; i < times; i++)
    {
        testArray[*shared] = thrcurrent;
        *shared = *shared + 1;
        yield();
    }
}

thread test_schedule(bool verbose)
{
    char str[50];
    uchar testArray[4 * TIMES];
    uchar expectedResults[4 * TIMES];
    int shared = 0;
    tid_typ atid, btid, ctid, dtid, i;
    int btid_remaining, dtid_remaining;
    bool passed = TRUE;

    /* disable interrupts so we execute in known order */
    disable();
    ready(atid = create((void *)t4, INITSTK, 31, "PRIORITY-A",
                        3, TIMES, testArray, &shared), 0);
    ready(btid = create((void *)t4, INITSTK, 32, "PRIORITY-B",
                        3, TIMES, testArray, &shared), 0);
    ready(ctid = create((void *)t4, INITSTK, 34, "PRIORITY-C",
                        3, TIMES, testArray, &shared), 0);
    ready(dtid = create((void *)t4, INITSTK, 32, "PRIORITY-D",
                        3, TIMES, testArray, &shared), 0);

    /* Run the tests by yielding the processor */
    yield();

    /* Generate expected results and compare */

    for (i = 0; i < TIMES; i++)
    {
        expectedResults[i] = ctid;
    }

    /* Since threads may be rescheduled before they can disable interrupts, the
     * exact order than threads B and D, which have equal priority, execute in
     * is unpredictable.  To avoid spurious test failures, we accept them
     * executing in any order.  */
    btid_remaining = TIMES;
    dtid_remaining = TIMES;
    for (i = TIMES; i < 3 * TIMES; i++)
    {
        if (testArray[i] == btid && btid_remaining > 0) {
            expectedResults[i] = btid;
            btid_remaining--;
        } else {
            expectedResults[i] = dtid;
            dtid_remaining--;
        }
    }

    for (i = 3 * TIMES; i < 4 * TIMES; i++)
    {
        expectedResults[i] = atid;
    }

    for (i = 0; i < 4 * TIMES; i++)
    {
        if (expectedResults[i] != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, expectedResults[i], testArray[i]);
            testFail(verbose, str);
        }
    }

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
