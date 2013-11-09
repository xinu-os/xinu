/**
 * @file     xsh_ethstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <conf.h>

#if NETHER

#include <device.h>
#include <ether.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @ingroup shell
 *
 * Shell command (ethstat ETHNUM) provides information about the current
 * status of an Ethernet device.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_ethstat(int nargs, char *args[])
{
    ushort dev, throughput = FALSE;
    int n;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s <ETHNUM>\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays statistics for ethernet <ETHNUM>.\n");
        printf("Options:\n");
        printf
            ("\t-t\t\tdisplay throughput statistics for ethernet <ETHNUM> ");
        printf("every second\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        printf("\t<ETHNUM>\tEthernet device number.\n");
        return 1;
    }

    /* No arguments to ethstat gives statistics for all interfaces */
    if (nargs < 2)
    {
        for (n = 0; n < NETHER; n++)
        {
            etherStat(n);
        }
        return 0;
    }
    else if (2 == nargs)
    {
        dev = atoi(args[1]);
    }
    else if (3 == nargs)
    {
        if (strcmp(args[1], "-t") != 0)
        {
            fprintf(stderr, "Invalid argument '%s', try %s --help\n",
                    args[1], args[0]);
            return 1;
        }
        dev = atoi(args[2]);
        throughput = TRUE;
    }
    else                        //if (nargs > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Check for valid device */
    if (dev >= NETHER)
    {
        fprintf(stderr, "%s: (%d) No such ethernet device\n", args[0],
                dev);
        return 1;
    }

    if (throughput)
    {
        etherThroughput(dev);
        return 0;
    }

    etherStat(dev);

    return 0;
}

#endif /* NETHER */
