/**
 * @file     xsh_telnetserver.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <thread.h>
#include <device.h>
#include <telnet.h>
#include <ipv4.h>
#include <network.h>
#include <ether.h>

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
 * Shell command (telnetserver).
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return 0 for success, 1 for error
 */
shellcmd xsh_telnetserver(int nargs, char *args[])
{
    int descrp, port, i, spawntelnet;
    struct thrent *thrptr;
    char thrname[TNMLEN];

    bzero(thrname, TNMLEN);
    /* parse arguments to find port number */
    if ((2 == nargs) && (strcmp(args[1], "--help") == 0))
    {
        printf("Usage: %s [-d device] [-p port] [-h]\n\n", args[0]);
        printf("Description:\n");
        printf
            ("\tSpawns a telnet server, providing remote shell access\n");
        printf("Options:\n");
        printf
            ("\t-d device\tdevice to listen for traffic. (default: ETH0)\n");
        printf
            ("\t-p port\t\tport on which the server listens. (default: 23)\n");
        printf("\t-h \t\thalt server.\n");
        printf("\t--help\t\tdisplay this help information and exit\n");
        return 0;
    }

    /* Halt telnet server */
    if ((2 == nargs) && (0 == strcmp(args[1], "-h")))
    {
        /* Kill all main telnet server threads */
        for (i = 0; i < NTHREAD; i++)
        {
            thrptr = &thrtab[i];
            if (thrptr->state == THRFREE)
            {
                continue;
            }

            if (0 == strncmp(thrptr->name, "telnetServ_", 11))
            {
                kill(i);
            }
        }

#if NTELNET
        semfree(telnettab[0].killswitch);
        telnettab[0].killswitch = semcreate(0);
#endif                          /* NTELNET */
        return 0;
    }

    if (nargs > 5)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try %s --help for usage\n", args[0]);
        return SHELL_ERROR;
    }

    /* assume default device and port */
    descrp = (ethertab[0].dev)->num;
    port = TELNET_PORT;

    /* set user options if specified */
    for (i = 1; i < nargs; i++)
    {
        if (strcmp(args[i], "-d") == 0)
        {
            i++;
            if (i >= nargs)
                return argErr(args[0], "");
            descrp = getdev(args[i]);
        }
        else if (strcmp(args[i], "-p") == 0)
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

    /* spawn three servers */
#if NTELNET
    for (i = 0; i < NTELNET; i++)
    {
        spawntelnet = telnetAlloc();
        sprintf(thrname, "telnetServ_%d\0", (spawntelnet - TELNET0));
        TELNET_TRACE("Spawning %s on %d", thrname, spawntelnet - TELNET0);
        ready(create((void *)telnetServer, INITSTK, INITPRIO, thrname,
                     4, descrp, port, spawntelnet, "SHELL2"),
              RESCHED_YES);
    }
#endif

    return SHELL_OK;
}
#endif /* NETHER */
