/*
 * @file     xsh_netstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <network.h>

#if NETHER
static void netStat(struct netif *);

/**
 * @ingroup shell
 *
 * Shell command (netstat).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_netstat(int nargs, char *args[])
{

    int i;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays Network Information\n");
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
    if (nargs < 1)
    {
        fprintf(stderr, "%s: too few arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }

#if NNETIF
    for (i = 0; i < NNETIF; i++)
    {
        netStat(&netiftab[i]);
    }
#else
    i = 0;
    netStat(NULL);
#endif

    return OK;
}

static void netStat(struct netif *netptr)
{
    device *pdev;
    char strA[20];
    char strB[20];

    /* Skip interface if not allocated */
    if ((NULL == netptr) || (netptr->state != NET_ALLOC))
    {
        return;
    }

    /* Setup pointer to underlying device */
    pdev = (device *)&devtab[netptr->dev];
    printf("%s:\n", pdev->name);
    netaddrsprintf(strA, &netptr->hwaddr);
    printf("\t");
    printf("HW Addr: %s\n", strA);
    netaddrsprintf(strA, &netptr->ip);
    netaddrsprintf(strB, &netptr->mask);
    printf("\t");
    printf("IP Addr: %-15s   Mask: %-15s\n", strA, strB);
    netaddrsprintf(strA, &netptr->gateway);
    netaddrsprintf(strB, &netptr->ipbrc);
    printf("\t");
    printf("Gateway: %-15s   Bcast IP: %-15s\n", strA, strB);
    printf("\t");
    printf("MTU: %-19d   Link Hdr Len: %d\n", netptr->mtu,
           netptr->linkhdrlen);
    printf("\t");
    printf("Num Rcv: %-15d   Num Proc: %d\n", netptr->nin, netptr->nproc);

    return;
}
#endif /* NETHER */
