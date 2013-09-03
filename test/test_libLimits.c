#include <stddef.h>
#include <limits.h>
#include <stdio.h>
#include <testsuite.h>

/**
 * Tests the limits.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libLimits(bool verbose)
{
    char c = 0;
    uchar uc = 0;
    signed char sc = 0;
    short s = 0;
    ushort us = 0;
    int i = 0;
    uint ui = 0;
    long l = 0;
    ulong ul = 0;

    bool passed = TRUE;

    /* NOTE: a regular 'char' may be either signed or unsigned.  */

    /* maximum value of char */
    testPrint(verbose, "Maximum value of char");
    c = CHAR_MAX;
    c++;
    failif((CHAR_MIN != c), "too small");

    /* minimum value of char */
    testPrint(verbose, "Minimum value of char");
    c = CHAR_MIN;
    c--;
    failif((CHAR_MAX != c), "too large");

    /* maximum value of signed char */
    testPrint(verbose, "Maximum value of signed char");
    sc = SCHAR_MAX;
    sc++;
    failif((SCHAR_MIN != sc), "too small");

    /* minimum value of char */
    testPrint(verbose, "Minimum value of signed char");
    sc = SCHAR_MIN;
    sc--;
    failif((SCHAR_MAX != sc), "too large");

    /* maximum value of unsigned char */
    testPrint(verbose, "Maximum value of unsigned char");
    uc = UCHAR_MAX;
    uc++;
    failif((0 != uc), "too small");

    /* maximum value of short */
    testPrint(verbose, "Maximum value of short");
    s = SHRT_MAX;
    s++;
    failif((SHRT_MIN != s), "too small");

    /* minimum value of short */
    testPrint(verbose, "Minimum value of short");
    s = SHRT_MIN;
    s--;
    failif((SHRT_MAX != s), "too large");

    /* maximum value of unsigned short */
    testPrint(verbose, "Maximum value of unsigned short");
    us = USHRT_MAX;
    us++;
    failif((0 != us), "too small");

    /* maximum value of int */
    testPrint(verbose, "Maximum value of int");
    i = INT_MAX;
    i++;
    failif((INT_MIN != i), "too small");

    /* minimum value of int */
    testPrint(verbose, "Minimum value of int");
    i = INT_MIN;
    i--;
    failif((INT_MAX != i), "too large");

    /* maximum value of unsigned int */
    testPrint(verbose, "Maximum value of unsigned int");
    ui = UINT_MAX;
    ui++;
    failif((0 != ui), "too small");

    /* maximum value of long */
    testPrint(verbose, "Maximum value of long");
    l = LONG_MAX;
    l++;
    failif((LONG_MIN != l), "too small");

    /* minimum value of long */
    testPrint(verbose, "Minimum value of long");
    l = LONG_MIN;
    l--;
    failif((LONG_MAX != l), "too large");

    /* maximum value of unsigned long */
    testPrint(verbose, "Maximum value of unsigned long");
    ul = ULONG_MAX;
    ul++;
    failif((0 != ul), "too small");

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
