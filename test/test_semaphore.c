#include <stddef.h>
#include <thread.h>
#include <semaphore.h>
#include <stdio.h>
#include <testsuite.h>

void test_semWaiter(semaphore s, int times, uchar *testResult)
{
    while (times > 0)
    {
        wait(s);
        (*testResult)++;
        times--;
    }
}

bool test_checkSemCount(semaphore s, short c, bool verbose)
{
    char msg[50];

    if (!isbadsem(s) && c != semcount(s))
    {
        sprintf(msg, "count = %d, not %d", semcount(s), c);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

bool test_checkProcState(tid_typ tid, uchar state, bool verbose)
{
    char msg[50];

    if (state != thrtab[tid].state)
    {
        sprintf(msg, "tid %d state %d, not %d",
                tid, thrtab[tid].state, state);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

bool test_checkResult(uchar testResult, uchar expected, bool verbose)
{
    char msg[80];

    if (expected != testResult)
    {
        sprintf(msg,
                "process didn't seem to wait, expected %d, saw %d",
                expected, testResult);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

thread test_semaphore(bool verbose)
{
    tid_typ atid;
    bool passed = TRUE;
    semaphore s;
    uchar testResult = 0;
    char msg[50];

    /* Single semaphore tests */
    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(0);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 0, verbose))
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
    if (test_checkProcState(atid, THRWAIT, verbose)
        && test_checkSemCount(s, -1, verbose)
        && test_checkResult(testResult, 0, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    signal(s);

    testPrint(verbose, "Signal semaphore: ");
    if (test_checkProcState(atid, THRFREE, verbose)
        && test_checkSemCount(s, 0, verbose)
        && test_checkResult(testResult, 1, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    signaln(s, 5);
    testPrint(verbose, "Signaln semaphore (valid count): ");
    if (test_checkProcState(atid, THRFREE, verbose)
        && test_checkSemCount(s, 5, verbose)
        && test_checkResult(testResult, 1, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    /* Free semaphore, single semaphore tests */
    semfree(s);

    testPrint(verbose, "Signal bad semaphore: ");
    if (SYSERR == signal(s))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Signaln bad semaphore: ");
    if (SYSERR == signaln(s, 4))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    /* Process A should be dead, but in case the test failed. */
    kill(atid);

    /* General semaphore pass/fail */
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
