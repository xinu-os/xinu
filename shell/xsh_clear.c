/**
 * @file     xsh_clear.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @ingroup shell
 *
 * Shell command (clear) clears the terminal using vt100 terminal control.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_clear(int nargs, char *args[])
{
    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tClears the terminal.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");
        return 1;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n",
                args[0]);
        return 1;
    }

    printf("\033[2J\033[H\n");
    return 0;
}
