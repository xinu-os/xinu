/**
 * @file     main.c
 *
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
#ifdef TTY1
    /* Associate TTY1 with second serial port. */
    open(TTY1, SERIAL1);
#endif

#ifdef NETHER
    /* Open all ethernet devices */
    {
        int i;
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
    }
#endif

#if HAVE_SHELL /* if shell enabled in platformVars APPCOMPS  */

    /* Launch one shell process for each TTY device. */
#ifdef CONSOLE
#ifdef FRAMEBUF
    ready(create
          ((void *)shell, INITSTK, INITPRIO, "SHELL0", 3,
           CONSOLE, FRAMEBUF, FRAMEBUF), RESCHED_NO);
#else 
    ready(create
          ((void *)shell, INITSTK, INITPRIO, "SHELL0", 3,
           CONSOLE, CONSOLE, CONSOLE), RESCHED_NO);
#endif /* FRAMEBUF */
#endif /* CONSOLE */

#ifdef TTY1
    ready(create
          ((void *)shell, INITSTK, INITPRIO, "SHELL1", 3,
           TTY1, TTY1, TTY1), RESCHED_NO);
#endif

#endif /* HAVE_SHELL */
    
    resched();


    return 0;
}
