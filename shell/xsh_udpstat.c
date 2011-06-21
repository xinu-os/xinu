/*
 * @file     xsh_tcpstat.c
 * @provides xsh_tcpstat
 *
 * $Id: xsh_udpstat.c 2132 2009-11-17 23:22:38Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <udp.h>

static void udpStat(struct udp *);

/**
 * Shell command (udpstat).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_udpstat(int nargs, char *args[])
{

    int i;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays UDP device information\n");
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

#if NUDP
    for (i = 0; i < NUDP; i++)
    {
        udpStat(&udptab[i]);
    }
#else
    i = 0;
    udpStat(NULL);
#endif

    return OK;
}

static void udpStat(struct udp *udpptr)
{
    device *pdev;
    char strA[20];

    if (NULL == udpptr)
    {
        return;
    }

    /* Skip interface if not allocated */
    if (udpptr->state != UDP_OPEN)
    {
        return;
    }

    /* Setup pointer to underlying device */
    pdev = udpptr->dev;
    printf("%-10s ", pdev->name);

    /* Connection details */
    netaddrsprintf(strA, &udpptr->localip);
    printf("Local  Port: %-5d IP: %-15s\n", udpptr->localpt, strA);
    printf("           ");
    netaddrsprintf(strA, &udpptr->remoteip);
    printf("Remote Port: %-5d IP: %-15s\n", udpptr->remotept, strA);
}
