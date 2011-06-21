#include <stdio.h>
#include <stddef.h>
#include <testsuite.h>

/**
 * Example of a test program for the Xinu testsuite.  Beyond this file you
 * must add an entry to the testtab in testhelper.c and a prototype in
 * testsuite.h.
 */
thread test_stub(bool verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;

    /* This is the first "subtest" of this suite */
    testPrint(verbose, "subtest one");
    test = ionull();
    failif((test != OK), "ionull not OK");

    /* This is the second "subtest" of this suite */
    testPrint(verbose, "subtest two");
    value = ioerr();
    failif((value != SYSERR), "ioerr not SYSERR");

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
