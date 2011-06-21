/**
 * @file     xsh_reset.c
 * @provides xsh_reset.
 *
 * $Id: xsh_reset.c 2157 2010-01-19 00:40:07Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <gpio.h>
#include <stdio.h>
#include <string.h>

/**
 * Shell command (reset) performs a soft restart of the router.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_reset(int nargs, char *args[])
{
    struct gpio_csreg *pgcsr;   /* pointer to gpio registers     */

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tPerforms a soft reset of the router.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");
        return 1;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

#ifdef GPIO_BASE
    /* Initialize pointers */
    pgcsr = (struct gpio_csreg *)GPIO_BASE;

    /* Reset router */
    pgcsr->input |= GPIO_BUT_RESET;
#else
    pgcsr = NULL;
#endif                          /* GPIO_BASE */

    return 0;
}
