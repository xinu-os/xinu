/**
 * @file     xsh_sleep.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/**
 * @ingroup shell
 *
 * Shell command (sleep DELAY) delay for a given number of seconds.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_sleep(int nargs, char *args[])
{
    uint delay;                 /* time to sleep */

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s <DELAY>\n\n", args[0]);
        printf("Description:\n");
        printf("\tSleep for <DELAY> seconds.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");
        return 0;
    }

    /* Check for correct number of arguments */
    if (nargs < 2)
    {
        fprintf(stderr, "%s: missing operand\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }
    if (nargs > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Calculate delay and sleep */
    if (1 != sscanf(args[1], "%u", &delay) || delay > UINT_MAX / 1000)
    {
        fprintf(stderr, "%s: invalid delay of \"%s\" seconds\n",
                args[0], args[1]);
        return 1;
    }

    /* convert delay to milliseconds */
    if (SYSERR == sleep(delay * 1000))
    {
        return 1;
    }

    return 0;
}
