/**
 * @file     xsh_testsuite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <conf.h>

#if HAVE_TESTSUITE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <testsuite.h>
#include <thread.h>

static void testItem(int, bool);
static void help(char *command);

/**
 * @ingroup shell
 *
 * Shell command (testsuite) provides a menu of standard Xinu component
 * tests.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_testsuite(int nargs, char *args[])
{
    int i;
    bool verbose = FALSE;
    bool testall = TRUE;
    int testnum;

    /* Parse args */
    for (i = 1; i < nargs; i++)
    {
        if (0 == strcmp(args[i], "--help"))
        {
            help(args[0]);
            return 1;
        }
        else if (0 == strcmp(args[i], "-v"))
        {
            verbose = TRUE;
        }
        else
        {
            testnum = atoi(args[i]) - 1;
            testItem(testnum, verbose);
            testall = FALSE;
        }
    }

    if (TRUE == testall)
    {
        for (i = 0; i < ntests; i++)
        {
            testItem(i, verbose);
        }
    }

    return 0;
}

static void help(char *command)
{
    int i;

    printf("Usage: %s <TESTNUM>\n\n", command);
    printf("Description:\n");
    printf("\tSuite of standard Xinu component tests.\n");
    printf("Options:\n");
    printf("\t--help\t\tdisplay this help and exit\n");
    printf("\t<TESTNUM>\tif specified, specific test number to run\n");
    printf("\t-v\t\tverbose information\n");

    for (i = 0; i < ntests; i++)
    {
        printf("%2d. %s\n", i + 1, testtab[i].name);
    }
}

static void testItem(int testnum, bool verbose)
{
    uint child = SYSERR;

    if (0 <= testnum && testnum < ntests)
    {
        child =
            create(testtab[testnum].test, INITSTK,
                   30, testtab[testnum].name, 1, verbose);

        if (SYSERR != child)
        {
            printf("Test Suite %2d: %s%s", testnum + 1,
                   testtab[testnum].name, (verbose ? "\n" : ""));

            /* set file descriptors */
            thrtab[child].fdesc[0] = stdin;
            thrtab[child].fdesc[1] = stdout;
            thrtab[child].fdesc[2] = stderr;

            /* Clear waiting message; Reschedule; */
            while (recvclr() != NOMSG);
            ready(child, RESCHED_YES);

            /* Wait for command process to finish */
            while (receive() != child);
        }
    }
    else
    {
        fprintf(stderr, "testsuite: (%d) No such test.\n", testnum + 1);
    }
}

#endif /* HAVE_TESTSUITE */
