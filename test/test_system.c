#include <stdio.h>
#include <stddef.h>
#include <testsuite.h>
#include <device.h>
#include <conf.h>

/**
 * Example of a test program for the Xinu testsuite.  Beyond this file you
 * must add an entry to the testtab in testhelper.c and a prototype in
 * testsuite.h.
 */
thread test_system(bool verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;

    testPrint(verbose, "Checking gettid()");
    failif((thrcurrent != gettid()),
           "gettid() does not return thrcurrent");

    testPrint(verbose, "Checking getprio()");
    failif((thrtab[gettid()].prio != getprio(gettid())),
           "getprio() does not return prio");
    failif((SYSERR != getprio(-1)),
           "getprio() does not return SYSERR for bad tid");
    failif((SYSERR != getprio(NTHREAD)),
           "getprio() does not return SYSERR for bad tid");

    testPrint(verbose, "Checking getdev()");
    failif((SERIAL0 != getdev("SERIAL0")), "getdev failed");
    failif((SYSERR != getdev("BADDEV")), "getdev failed");

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
