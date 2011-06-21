#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <testsuite.h>

/**
 * Tests the ctype.h header in the Xinu Standard Library.
 * @return OK when testing is complete
 */
thread test_libCtype(bool verbose)
{
    char c = NULL;
    char str[50];
    bool passed = TRUE;

    testPrint(verbose, "Upper alpha");
    for (c = 'A'; c <= 'Z'; c++)
    {
        if ((FALSE == isalpha(c))
            || (FALSE == isalnum(c))
            || (FALSE == isupper(c)) || (FALSE == islower(tolower(c))))
        {
            sprintf(str, "%c should be upper alpha", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > 'Z')
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Lower alpha");
    for (c = 'a'; c <= 'z'; c++)
    {
        if ((FALSE == isalpha(c))
            || (FALSE == isalnum(c))
            || (FALSE == islower(c)) || (FALSE == isupper(toupper(c))))
        {
            sprintf(str, "%c should be lower alpha", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > 'z')
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Upper hexadecimal digits");
    for (c = 'A'; c <= 'F'; c++)
    {
        if (FALSE == isxdigit(c))
        {
            sprintf(str, "%c should be hexadecimal digit", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > 'F')
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Lower hexadecimal digits");
    for (c = 'a'; c <= 'f'; c++)
    {
        if (FALSE == isxdigit(c))
        {
            sprintf(str, "%c should be hexadecimal digit", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > 'f')
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Digits");
    for (c = '0'; c <= '9'; c++)
    {
        if ((FALSE == isdigit(c))
            || (FALSE == isalnum(c)) || (FALSE == isxdigit(c)))
        {
            sprintf(str, "%c should be digit", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > '9')
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Punctuation");
    char *punct = "!\"#$%&’()*+,-./:;<=>?@[\\]^_`{|}~";

    while ('\0' != *punct)
    {
        if (FALSE == ispunct(c))
        {
            sprintf(str, "%c should be punctuation", c);
            testFail(verbose, str);
            passed = FALSE;
        }
        punct++;
    }
    if ('\0' == *punct)
    {
        testPass(verbose, "");
    }

    testPrint(verbose, "Printable");
    for (c = '\0'; c <= '~'; c++)
    {
        if ((FALSE == isprint(c)) && (FALSE == iscntrl(c)))
        {
            sprintf(str, "%c should be printable", c);
            testFail(verbose, str);
            passed = FALSE;
        }
    }
    if (c > '~')
    {
        testPass(verbose, "");
    }

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
