/**
 * @file     xsh_reset.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <clock.h>
#include <gpio.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <watchdog.h>

/**
 * @ingroup shell
 *
 * Shell command (reset) performs a soft restart of the router.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_reset(int nargs, char *args[])
{
#ifdef GPIO_BASE
    struct gpio_csreg *pgcsr;   /* pointer to gpio registers     */
#endif

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tPerforms a soft reset of the system.\n");
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

#if defined(_XINU_PLATFORM_ARM_RPI_)
    /* Set watchdog timer to elapse 1 millisecond from now, then wait for it to
     * go off.  */
    watchdogset(1);
    mdelay(1000);
#elif defined(GPIO_BASE)
    /* Initialize pointers */
    pgcsr = (struct gpio_csreg *)GPIO_BASE;

    /* Reset router */
    pgcsr->input |= GPIO_BUT_RESET;
#endif

    return 1;
}
