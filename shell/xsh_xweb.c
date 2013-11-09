/**
 * @file     xsh_webserver.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>

#include <ether.h>
#include <http.h>
#include <semaphore.h>
#include <string.h>
#include <thread.h>

#if NETHER
/**
 * @ingroup shell
 *
 * Shell command (webserver).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return 0 for success, 1 for error
 */
shellcmd xsh_xweb(int nargs, char *args[])
{
    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s [<DEVICE>|-h]\n\n", args[0]);
        printf("Description:\n");
        printf("\tSpawn XWeb thread.\n");
        printf("\tThis thread will respond to HTTP requests and is\n");
        printf("\tused to provide a web interface for Embedded Xinu\n");
        printf("\tWarning: At least one HTTP device must exist to run\n");
        printf("\tXWeb thread.\n");
        printf("Options:\n");
        printf("\t<DEVICE>\tUnderlying device (default: %s).\n",
               (ethertab[0].dev)->name);
        printf("\t-h\t\tHalt all XWeb threads and terminate Xweb.\n");
        printf("\t--help\t\tdisplay this help information\n\n");

        return 0;
    }

#ifdef NHTTP

    int i = 0;
    int descrp = 0;
    struct netif *interface = NULL;
    struct thrent *thrptr = NULL;
    semaphore oldsem = 0;

    /* Halt XWeb thread */
    if (nargs == 2 && strcmp(args[1], "-h") == 0)
    {
        /* Kill all main XWeb threads */
        for (i = 0; i < NTHREAD; i++)
        {
            thrptr = &thrtab[i];
            if (thrptr->state == THRFREE)
            {
                continue;
            }

            if (0 == strncmp(thrptr->name, "XWeb_", 5))
            {
                kill(i);
            }
        }

        /* Signal to kill any spawned threads */
        for (i = 0; i < NHTTP; i++)
        {
            signal(httptab[i].closeall);
        }

        oldsem = activeXWeb;
        activeXWeb = semcreate(1);
        semfree(oldsem);
        return 0;
    }

    if (nargs == 2)
    {
        descrp = getdev(args[1]);
        if (SYSERR == descrp)
        {
            fprintf(stderr, "%s is not a valid device.\n", args[1]);
            return 1;
        }
    }
    else
    {
        /* Assume first ethernet interface */
        descrp = (ethertab[0].dev)->num;
    }

    interface = netLookup(descrp);
    if (NULL == interface)
    {
        return 1;
    }

    /* Start XWeb */
    ready(create((void *)httpServerKickStart, INITSTK, INITPRIO,
                 "XWebKickStart", 1, descrp), RESCHED_YES);
    return 0;

#else
    printf("At least one HTTP device must exist to run XWeb.\n");
    return 1;
#endif
}
#endif /* NETHER */
