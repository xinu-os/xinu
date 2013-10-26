/*
 * @file     xsh_tcpstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <tcp.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (tcpstat).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_tcpstat(int nargs, char *args[])
{

    int i;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays TCP socket information\n");
        printf("Options:\n");
        printf("\t--help\tdisplay this help and exit\n");
        return OK;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }

#if NTCP
    for (i = 0; i < NTCP; i++)
    {
        tcpStat(&tcptab[i]);
    }
#else
    i = 0;
    tcpStat(NULL);
#endif                          /* NTCP */

    return OK;
}
#endif /* NETHER */
