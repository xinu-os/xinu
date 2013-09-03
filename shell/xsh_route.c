/**
 * @file     xsh_route.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ipv4.h>
#include <network.h>
#include <route.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (route) displays routing information.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_route(int nargs, char *args[])
{
    int i;
    char c[32];
    device *pdev;
    struct netif *netptr;

    struct netaddr dst;
    struct netaddr mask;
    struct netaddr gateway;

    /* Check for correct number of arguments */
    if (nargs > 6)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("\nUsage: %s ", args[0]);
        printf("[add <DESTINATION> <GATEWAY> <MASK> <INTERFACE>] ");
        printf("[del <DESTINATION>]\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays routing table\n");
        printf("Options:\n");
        printf("\tadd <DESTINATION> <GATEWAY> <MASK> <INTERFACE>\n");
        printf("\t\t\t\tadd route entry into table.\n");
        printf("\t\t\t\t(<INTERFACE> must be in all caps.)\n");
        printf("\tdel <DESTINATION>");
        printf("\tdelete route entry from table.\n");
        printf("\t--help\t\t\tdisplay this help and exit\n");
        return OK;
    }

    if (nargs == 6 && strncmp(args[1], "add", 4) == 0)
    {
        /* Parse destination */
        if (strncmp(args[2], "default", 7) == 0)
        {
            args[2] = "";
        }
        if (SYSERR == dot2ipv4(args[2], &dst))
        {
            fprintf(stderr, "%s is not a valid IPv4 address.\n", args[2]);
            return SYSERR;
        }

        /* Parse gateway */
        if (SYSERR == dot2ipv4(args[3], &gateway))
        {
            fprintf(stderr, "%s is not a valid IPv4 address.\n", args[3]);
            return SYSERR;
        }

        /* Parse mask */
        if (SYSERR == dot2ipv4(args[4], &mask))
        {
            fprintf(stderr, "%s is not a valid IPv4 address mask.\n",
                    args[4]);
            return SYSERR;
        }

        /* Parse interface */
#if NNETIF
        for (i = 0; i < NNETIF; i++)
        {
            if (NET_ALLOC == netiftab[i].state)
            {
                netptr = &netiftab[i];
                pdev = (device *)&devtab[netptr->dev];
                if (strncmp(pdev->name, args[5], 6) == 0)
                {
                    if (SYSERR == rtAdd(&dst, &gateway, &mask, netptr))
                    {
                        fprintf(stderr, "Failed to add route.\n");
                        return SYSERR;
                    }
                    return OK;
                }
            }
        }
#endif
        fprintf(stderr, "%s is not a valid network interface.\n",
                args[5]);
        return SYSERR;
    }
    else if (nargs == 3 && strncmp(args[1], "del", 3) == 0)
    {
        /* Parse destination */
        if (strncmp(args[2], "default", 7) == 0)
        {
            args[2] = "";
        }
        if (SYSERR == dot2ipv4(args[2], &dst))
        {
            fprintf(stderr, "%s is not a valid IPv4 address.\n", args[2]);
            return SYSERR;
        }

        if (SYSERR == rtRemove(&dst))
        {
            fprintf(stderr, "Failed to delete route.\n");
            return SYSERR;
        }
        return OK;
    }

    printf
        ("Destination     Gateway         Mask            Interface\r\n");

    for (i = 0; i < RT_NENTRY; i++)
    {
        if (RT_USED == rttab[i].state)
        {
            if (0 == rttab[i].masklen)
                sprintf(c, "default");
            else
                netaddrsprintf(c, &rttab[i].dst);
            printf("%-16s", c);

            netaddrsprintf(c, &rttab[i].gateway);
            if (strncmp(c, "NULL", 5) == 0)
                sprintf(c, "*");
            printf("%-16s", c);

            netaddrsprintf(c, &rttab[i].mask);
            printf("%-16s", c);

            netptr = rttab[i].nif;
            pdev = (device *)&devtab[netptr->dev];
            printf("%s\r\n", pdev->name);
        }
    }

    return 0;
}
#endif /* NETHER */
