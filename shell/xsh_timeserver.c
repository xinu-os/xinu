/**
 * @file     xsh_timeserver.c
 *
 */
/* Embedded Xinu, Copyright (C) 2010.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <thread.h>
#include <device.h>
#include <date.h>
#include <ipv4.h>
#include <network.h>
#include <ether.h>
#include <udp.h>

#if NETHER
static int argErr(char *command, char *arg)
{
    fprintf(stderr, "%s: missing or invalid argument\n", command);
    fprintf(stderr, "Try %s --help for usage\n", command);
    return SHELL_ERROR;
}

/**
 * @ingroup shell
 *
 * Shell command (timeserver).
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return 0 for success, 1 for error
 */
shellcmd xsh_timeserver(int nargs, char *args[])
{
    int descrp, port, i;

    /* parse arguments to find port number */
    if ((2 == nargs) && (strncmp(args[1], "--help", 7) == 0))
    {
        printf("Usage: %s [-d device] [-p port]\n\n", args[0]);
        printf("Description:\n");
        printf("\tSpawns a time server, providing remote system time\n");
        printf("Options:\n");
        printf
            ("\t-d device\tdevice to listen for traffic. (default: ETH0)\n");
        printf
            ("\t-p port\t\tport on which the server listens. (default: %d)\n",
             UDP_PORT_RDATE);
        printf("\t--help\t\tdisplay this help information and exit\n");
        return 0;
    }

    if (nargs > 4)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try %s --help for usage\n", args[0]);
        return SHELL_ERROR;
    }

    /* assume default device and port */
    descrp = (ethertab[0].dev)->num;
    port = UDP_PORT_RDATE;

    /* set user options if specified */
    for (i = 1; i < nargs; i++)
    {
        if (strncmp(args[i], "-d", 3) == 0)
        {
            i++;
            if (i >= nargs)
                return argErr(args[0], "");
            descrp = getdev(args[i]);
        }
        else if (strncmp(args[i], "-p", 3) == 0)
        {
            i++;
            if (i >= nargs)
                return argErr(args[0], "");
            port = atoi(args[i]);
        }
        else
        {
            return argErr(args[0], args[i]);
        }
    }

    /* verify the device is valid */
    if (isbaddev(descrp))
    {
        fprintf(stderr, "%s: invalid device.\n", args[0]);
        fprintf(stderr, "Try %s --help for usage\n", args[0]);
        return SHELL_ERROR;
    }

    /* verify the provided port is a valid number */
    if (port <= 0)
    {
        fprintf(stderr, "%s: invalid port\n", args[0]);
        fprintf(stderr, "Try %s --help for usage\n", args[0]);
        return SHELL_ERROR;
    }

    ready(create((void *)timeServer, INITSTK, INITPRIO, "TimeServer",
                 2, descrp, port), RESCHED_YES);

    return SHELL_OK;
}
#endif /* NETHER */
