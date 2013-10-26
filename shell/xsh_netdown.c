/**
 * @file     xsh_netdown.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <interrupt.h>
#include <ipv4.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (netdown) to stop a network interface.
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_netdown(int nargs, char *args[])
{
    int descrp;

    /* Help */
    if ((2 == nargs) && (0 == strcmp(args[1], "--help")))
    {
        printf("Usage:\n");
        printf("\t%s <DEVICE>\n", args[0]);
        printf("Description:\n");
        printf
            ("\tStops a network interface on the specified underlying\n");
        printf("\tdevice.\n");
        printf("Options:\n");
        printf("\t<DEVICE>\tunderlying device (ex. ETH0)\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        return SHELL_OK;
    }

    /* Verify number of arguments */
    if (nargs != 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return SHELL_ERROR;
    }

    /* Parse device */
    descrp = getdev(args[1]);
    if (SYSERR == descrp)
    {
        fprintf(stderr, "%s is not a valid device.\n", args[1]);
        return SHELL_ERROR;
    }

    if (SYSERR == netDown(descrp))
    {
        fprintf(stderr, "Failed to stop network interface on %s\n",
                args[1]);
        return SHELL_ERROR;
    }

    return SHELL_OK;
}
#endif /* NETHER */
