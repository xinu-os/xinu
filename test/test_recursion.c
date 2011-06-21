#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <testsuite.h>

#define TIMES 5

static void t5(int times, uchar *testArray, int *index)
{
    if (times > 0)
    {
        testArray[*index] = thrcurrent;
        *index = *index + 1;
        testArray[*index] = times;
        *index = *index + 1;
        yield();
        t5(times - 1, testArray, index);
    }
}

thread test_recursion(bool verbose)
{
    tid_typ atid, btid, ctid, dtid, i, j;
    bool passed = TRUE;
    uchar testArray[100];
    char str[50];
    int index = 0;

    ready(atid = create((void *)t5, INITSTK, 31, "RECURSION-A",
                        3, TIMES, testArray, &index), 0);
    ready(btid = create((void *)t5, INITSTK, 32, "RECURSION-B",
                        3, TIMES, testArray, &index), 0);
    ready(ctid = create((void *)t5, INITSTK, 34, "RECURSION-C",
                        3, TIMES, testArray, &index), 0);
    ready(dtid = create((void *)t5, INITSTK, 32, "RECURSION-D",
                        3, TIMES, testArray, &index), 0);
    yield();

//      for (i = 0; i < 8 * TIMES; i++)
//      { printf("testArray[%d] == %d\n", i, testArray[i]); }

    for (i = 0, j = TIMES; i < TIMES; i += 2, j--)
    {
        if (ctid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, ctid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES * 2, j = TIMES; i < 6 * TIMES; i += 4, j--)
    {
        if (btid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, btid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES * 2 + 2, j = TIMES; i < 6 * TIMES; i += 4, j--)
    {
        if (dtid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, dtid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = 6 * TIMES, j = TIMES; i < 8 * TIMES; i += 2, j--)
    {
        if (atid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, atid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
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
