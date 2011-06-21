#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <testsuite.h>

#define TIMES 5

static void t4(int times, uchar *testArray, int *shared)
{
    int i = 0;

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
    uchar testArray[20];
    int shared = 0;

    tid_typ atid, btid, ctid, dtid, i;
    bool passed = TRUE;

    ready(atid = create((void *)t4, INITSTK, 31, "PRIORITY-A",
                        3, TIMES, testArray, &shared), 0);
    ready(btid = create((void *)t4, INITSTK, 32, "PRIORITY-B",
                        3, TIMES, testArray, &shared), 0);
    ready(ctid = create((void *)t4, INITSTK, 34, "PRIORITY-C",
                        3, TIMES, testArray, &shared), 0);
    ready(dtid = create((void *)t4, INITSTK, 32, "PRIORITY-D",
                        3, TIMES, testArray, &shared), 0);
    yield();

    for (i = 0; i < TIMES; i++)
    {
        if (ctid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, ctid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES; i < 3 * TIMES; i += 2)
    {
        if (btid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, btid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES + 1; i < 3 * TIMES; i += 2)
    {
        if (dtid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, dtid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = 3 * TIMES; i < 4 * TIMES; i++)
    {
        if (atid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, atid, testArray[i]);
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
