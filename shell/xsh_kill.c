/**
 * @file     xsh_kill.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @ingroup shell
 *
 * Shell command (kill) terminates a thread.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_kill(int nargs, char *args[])
{
    tid_typ tid;                /* tid for thread to kill */

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s <PID>\n\n", args[0]);
        printf("Description:\n");
        printf("\tKills a thread number PID.\n");
        printf("Options:\n");
        printf("\t<PID>\tID of thread to kill\n");
        printf("\t--help\tdisplay this help and exit\n");
        return 0;
    }

    /* Check for correct number of arguments */
    if (nargs < 2)
    {
        fprintf(stderr, "%s: missing operand\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }
    if (nargs > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    tid = atoi(args[1]);

    /* Block killing of the null thread */
    if (tid == NULLTHREAD)
    {
        fprintf(stderr, "%s: (%d) Operation not permitted\n", args[0],
                tid);
        return 1;
    }

    /* Notify of killing of the shell */
    if (tid == gettid())
    {
        fprintf(stderr, "%s: Shell killed\n", args[0]);
        sleep(2000);
    }

    /* Kill thread, noting if kill failed */
    if (kill(tid) == SYSERR)
    {
        fprintf(stderr, "%s: (%d) No such thread\n", args[0], tid);
        return -1;
    }

    return 0;
}
