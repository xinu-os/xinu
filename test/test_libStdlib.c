#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <testsuite.h>

static int qsort_callback(const char *left, const char *right)
{
    if (*left > *right)
    {
        return 1;
    }
    else if (*left < *right)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static int cmp_uints(const void *p1, const void *p2)
{
    uint u1 = *(const uint*)p1;
    uint u2 = *(const uint*)p2;

    if (u1 < u2)
    {
        return -1;
    }
    else if (u1 == u2)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * Tests the stdlib.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libStdlib(bool verbose)
{
    int i;
    char list[10] = "BCADFEHGI";
    bool passed = TRUE;
    bool all_sorted;

    /* atoi */
    testPrint(verbose, "ASCII to integer");
    failif((-456 != atoi("-456"))
           || (123 != atoi("+123"))
           || (2147483647 != atoi("2147483647"))
           || (-1 != atoi("-1one"))
           || (2 != atoi("+2two"))
           || (3 != atoi("3three"))
           || (0 != atoi("-on1e"))
           || (0 != atoi("+tw2o")) || (0 != atoi("thre3e")), "");

    /* atol */
    testPrint(verbose, "ASCII to long");
    failif((-456 != atol("-456"))
           || (123 != atol("+123"))
           || (2147483647 != atol("2147483647"))
           || (-1 != atol("-1one"))
           || (2 != atol("+2two"))
           || (3 != atol("3three"))
           || (0 != atol("-one"))
           || (0 != atol("+two")) || (0 != atol("three")), "");

    /* qsort */
    testPrint(verbose, "Quick sort");
    qsort(list, sizeof(list) - 1, 1, (void *)qsort_callback);
    failif((0 != strncmp(list, "ABCDEFGHI", sizeof(list))), "");

    /* bzero */
    testPrint(verbose, "bzero");
    bzero(list, sizeof(list));
    for (i = 0; i < sizeof(list); i++)
    {
        if ('\0' != *(list + i))
        {
            testFail(verbose, "");
            passed = FALSE;
            break;
        }
    }
    if (i == sizeof(list))
    {
        testPass(verbose, "");
    }

    /* abs */
    testPrint(verbose, "Absolute value");
    failif((2147483647 != abs(-2147483647))
           || (123 != abs(123)) || (0 != abs(0)), "");

    /* labs */
    testPrint(verbose, "Long absolute value");
    failif((2147483647 != labs(-2147483647))
           || (123 != labs(123)) || (0 != labs(0)), "");

    /* rand */
    testPrint(verbose, "Random number generation");
    failif((rand() == rand())
           && (rand() == rand())
           && (rand() == rand())
           && (rand() == rand()), "that was unlikely");

    /* Qucksort some random arrays to try to find bugs in qsort() that depend on
     * particular inputs.  */
    all_sorted = TRUE;
    srand(1); /* Always use same "random" arrays. */
    for (i = 0; i < 50; i++)
    {
        uint j;
        uint len;

        len = (uint)rand() % 100 + 1;

        uint array[len];

        for (j = 0; j < len; j++)
        {
            array[j] = rand();
        }
        qsort(array, len, sizeof(array[0]), cmp_uints);

        for (j = 0; j < len - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                all_sorted = FALSE;
            }
        }
    }
    testPrint(verbose, "Quicksort (random arrays)");
    failif(!all_sorted, "failed to sort random arrays");

    /* malloc (in test_umemory.c) */

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
