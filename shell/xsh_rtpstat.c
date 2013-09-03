/*
 * @file     xsh_rtpstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>
#include <rtp.h>

static void rtpStat(struct rtp *);

/**
 * @ingroup shell
 *
 * Shell command (rtpstat).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_rtpstat(int nargs, char *args[])
{

    int i;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays RTP device information\n");
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

#if NRTP
    for (i = 0; i < NRTP; i++)
    {
        rtpStat(&rtptab[i]);
    }
#else
    i = 0;
    rtpStat(NULL);
#endif

    return OK;
}

static void rtpStat(struct rtp *rtpptr)
{
    device *pdev;
    char strA[20];

    if (NULL == rtpptr)
    {
        return;
    }

    /* Skip interface if not allocated */
    if (rtpptr->state != RTP_OPEN)
    {
        return;
    }

    /* Setup pointer to underlying device */
    pdev = rtpptr->dev;
    printf("%-10s ", pdev->name);

    /* Connection details */
    printf("RTP Version: \n" + RTP_VERSION);
    printf("Local  Port: %-5d\n", rtpptr->localpt);
    printf("           ");
    printf("Remote Port: %-5d\n", rtpptr->remotept);

    //TODO Handle RTCP Info Output when RTCP is ready
}
