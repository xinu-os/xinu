/**
 * @file     main.c
 * @provides main.
 *
 * $Id: main.c 2070 2009-09-18 22:36:02Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <ether.h>
#include <thread.h>
#include <device.h>

extern void shell(int);

int main(int argc, char **argv)
{
    int i = 0;

#ifdef TTY1
    /* Associate TTY1 with second serial port. */
    open(TTY1, SERIAL1);
#endif

#ifdef NETHER
    /* Open all ethernet devices */
    for (i = 0; i < NETHER; i++)
    {
        /* Open the underlying ethernet device */
        if (SYSERR == open(ethertab[i].dev->num))
        {
            kprintf
                ("Failed to open ethernet device %s\r\n",
                 ethertab[i].dev->name);
        }
    }
#else
    i = 0;
#endif

    /* Launch one shell process for each TTY device. */
#ifdef CONSOLE
    ready(create
          ((void *)shell, INITSTK, INITPRIO, "SHELL0", 3,
           CONSOLE, CONSOLE, CONSOLE), RESCHED_NO);
#endif
#ifdef TTY1
    ready(create
          ((void *)shell, INITSTK, INITPRIO, "SHELL1", 3,
           TTY1, TTY1, TTY1), RESCHED_NO);
#endif
    return 0;
}
