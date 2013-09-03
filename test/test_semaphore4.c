#include <stddef.h>
#include <thread.h>
#include <semaphore.h>
#include <stdio.h>
#include <testsuite.h>

#if NSEM
extern bool test_checkSemCount(semaphore s, short c);
extern bool test_checkProcState(tid_typ tid, uchar state);
extern bool test_checkResult(uchar testResult, uchar expected);

extern void test_semWaiter(semaphore s, int times, uchar *testResult);
#endif

thread test_semaphore4(bool verbose)
{
#if NSEM
    tid_typ atid;
    bool passed = TRUE;
    semaphore s;
    uchar testResult = 0;
    char msg[50];

    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(0);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 0))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    ready(atid =
          create((void *)test_semWaiter, INITSTK, 31,
                 "SEMAPHORE-A", 3, s, 1, &testResult), RESCHED_YES);

    testPrint(verbose, "Wait on semaphore: ");
    if (test_checkProcState(atid, THRWAIT)
        && test_checkSemCount(s, -1) && test_checkResult(testResult, 0))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    kill(atid);

    testPrint(verbose, "Kill waiting process: ");
    if (test_checkProcState(atid, THRFREE)
        && test_checkSemCount(s, 0) && test_checkResult(testResult, 0))
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

    semfree(s);

#else /* NSEM */
    testSkip(TRUE, "");
#endif /* NSEM == 0 */
    return OK;
}
