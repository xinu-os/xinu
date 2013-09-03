/**
 * @file     xsh_sleep.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    long delay;                 /* time to sleep */

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
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
    delay = atol(args[1]);
    if (delay < 0)
    {
        fprintf(stderr, "%s: invalid delay of %d seconds\n", args[0],
                delay);
    }

    /* convert delay to milliseconds */
    if (sleep(delay * 1000) == SYSERR)
    {
        return -1;
    }

    return 0;
}
