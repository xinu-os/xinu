#include <stddef.h>
#include <thread.h>
#include <semaphore.h>
#include <stdio.h>
#include <testsuite.h>

extern bool test_checkSemCount(semaphore s, short c);
extern bool test_checkProcState(tid_typ tid, uchar state);
extern bool test_checkResult(uchar testResult, uchar expected);

extern void test_semWaiter(semaphore s, int times, uchar *testResult);

thread test_semaphore3(bool verbose)
{
    tid_typ atid, btid;
    bool passed = TRUE;
    semaphore s;
    uchar testResult = 0;
    char msg[50];

    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(1);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    ready(atid =
          create((void *)test_semWaiter, INITSTK, 31,
                 "SEMAPHORE-A", 3, s, 1, &testResult), RESCHED_NO);
    ready(btid =
          create((void *)test_semWaiter, INITSTK, 31,
                 "SEMAPHORE-B", 3, s, 1, &testResult), RESCHED_YES);

    testPrint(verbose, "Wait on semaphore: ");
    /* Process A should be admitted, but B should wait. */
    if (test_checkProcState(atid, THRFREE)
        && test_checkProcState(btid, THRWAIT)
        && test_checkSemCount(s, -1) && test_checkResult(testResult, 1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    signal(s);

    /* Process B waited, so signal should release it. */
    testPrint(verbose, "Signal waiting semaphore: ");
    if (test_checkProcState(btid, THRFREE)
        && test_checkSemCount(s, 0) && test_checkResult(testResult, 2))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    if (TRUE == passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    /* Processes should be dead, but in case the test failed. */
    kill(atid);
    kill(btid);
    semfree(s);

    return OK;
}
