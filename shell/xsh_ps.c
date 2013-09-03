/**
 * @file     xsh_ps.c 
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <string.h>

/**
 * @ingroup shell
 *
 * Shell command (ps) outputs thread table information.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_ps(int nargs, char *args[])
{
    struct thrent *thrptr;      /* pointer to thread entry  */
    uchar i;                    /* temp variable            */

    /* readable names for PR* status in thread.h */
    char *pstnams[] = { "curr ", "free ", "ready", "recv ",
        "sleep", "susp ", "wait ", "rtim "
    };

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays a table of running threads.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");

        return 0;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Output thread table header */
/*    printf(
            "TID NAME         STATE PRIO PPID STACK BASE STACK PTR  STACK LEN \n");

    printf(
            "--- ------------ ----- ---- ---- ---------- ---------- ----------\n");
*/

    printf("%3s %-16s %5s %4s %4s %10s %-10s %10s\n",
           "TID", "NAME", "STATE", "PRIO", "PPID", "STACK BASE",
           "STACK PTR", "STACK LEN");


    printf("%3s %-16s %5s %4s %4s %10s %-10s %10s\n",
           "---", "----------------", "-----", "----", "----",
           "----------", "----------", " ---------");

    /* Output information for each thread */
    for (i = 0; i < NTHREAD; i++)
    {
        thrptr = &thrtab[i];
        if (thrptr->state == THRFREE)
        {
            continue;
        }

        printf("%3d %-16s %s %4d %4d 0x%08X 0x%08X %10d\n",
               i, thrptr->name,
               pstnams[(int)thrptr->state - 1],
               thrptr->prio, thrptr->parent,
               thrptr->stkbase, thrptr->stkptr, thrptr->stklen);
    }

    return 0;
}
