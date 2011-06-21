/**
 * @file xsh_vlanstat.c
 * @provides xsh_vlanstat.
 *
 * $Id: xsh_vlanstat.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <string.h>

int vlanStat(void);

/**
 * Provides info about the current status of VLAN switch.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_vlanstat(int nargs, char *args[])
{
    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description\n");
        printf("\tDisplays status of VLAN switch.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");
        return 1;
    }

    /* Check for correct number of arguments */
    if (nargs == 1)
    {
        return vlanStat();
    }
    else
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }
}
