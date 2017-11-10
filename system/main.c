/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <device.h>
#include <ether.h>
#include <platform.h>
#include <shell.h>
#include <stdio.h>
#include <thread.h>
#include <version.h>

static void print_os_info(void);

/**
 * Main thread.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.  The default is designed to do something reasonable
 * on all platforms based on the devices and features configured.
 */
thread main(void)
{
#if HAVE_SHELL
    int shelldevs[4][3];
    uint nshells = 0;
#endif

    /* Print information about the operating system  */
    print_os_info();

    /* Open all ethernet devices */
#if NETHER
    {
        uint i;

        for (i = 0; i < NETHER; i++)
        {
            if (SYSERR == open(ethertab[i].dev->num))
            {
                kprintf("WARNING: Failed to open %s\r\n",
                        ethertab[i].dev->name);
            }
        }
    }
#endif /* NETHER */

    /* Set up the first TTY (CONSOLE)  */
#if defined(CONSOLE) && defined(SERIAL0)
    if (OK == open(CONSOLE, SERIAL0))
    {
  #if HAVE_SHELL
        shelldevs[nshells][0] = CONSOLE;
        shelldevs[nshells][1] = CONSOLE;
        shelldevs[nshells][2] = CONSOLE;
        nshells++;
  #endif
    }
    else
    {
        kprintf("WARNING: Can't open CONSOLE over SERIAL0\r\n");
    }
#elif defined(SERIAL0)
  #warning "No TTY for SERIAL0"
#endif

    /* Set up the second TTY (TTY1) if possible  */
#if defined(TTY1)
  #if defined(KBDMON0)
    /* Associate TTY1 with keyboard and use framebuffer output  */
    if (OK == open(TTY1, KBDMON0))
    {
    #if HAVE_SHELL
        shelldevs[nshells][0] = TTY1;
        shelldevs[nshells][1] = TTY1;
        shelldevs[nshells][2] = TTY1;
        nshells++;
    #endif
    }
    else
    {
        kprintf("WARNING: Can't open TTY1 over KBDMON0\r\n");
    }
  #elif defined(SERIAL1)
    /* Associate TTY1 with SERIAL1  */
    if (OK == open(TTY1, SERIAL1))
    {
    #if HAVE_SHELL
        shelldevs[nshells][0] = TTY1;
        shelldevs[nshells][1] = TTY1;
        shelldevs[nshells][2] = TTY1;
        nshells++;
    #endif
    }
    else
    {
        kprintf("WARNING: Can't open TTY1 over SERIAL1\r\n");
    }
  #endif /* SERIAL1 */
#else /* TTY1 */
  #if defined(KBDMON0)
    #warning "No TTY for KBDMON0"
  #elif defined(SERIAL1)
    #warning "No TTY for SERIAL1"
  #endif
#endif /* TTY1 */

    /* Start shells  */
#if HAVE_SHELL
    {
        uint i;
        char name[16];

        for (i = 0; i < nshells; i++)
        {
            sprintf(name, "SHELL%u", i);
            if (SYSERR == ready(create
                                (shell, INITSTK, INITPRIO, name, 3,
                                 shelldevs[i][0],
                                 shelldevs[i][1],
                                 shelldevs[i][2]),
                                RESCHED_NO))
            {
                kprintf("WARNING: Failed to create %s", name);
            }
        }
    }
#endif

    return 0;
}

/* Start of kernel in memory (provided by linker)  */
extern void _start(void);

static void print_os_info(void)
{
    kprintf(VERSION);
    kprintf("\r\n\r\n");

#ifdef DETAIL
    /* Output detected platform. */
    kprintf("Processor identification: 0x%08X\r\n", cpuid);
    kprintf("Detected platform as: %s, %s\r\n\r\n",
            platform.family, platform.name);
#endif

    /* Output Xinu memory layout */
    kprintf("%10d bytes physical memory.\r\n",
            (ulong)platform.maxaddr - (ulong)platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (ulong)platform.minaddr, (ulong)(platform.maxaddr - 1));
#endif


    kprintf("%10d bytes reserved system area.\r\n",
            (ulong)_start - (ulong)platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (ulong)platform.minaddr, (ulong)_start - 1);
#endif

    kprintf("%10d bytes Xinu code.\r\n", (ulong)&_etext - (ulong)_start);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (ulong)_start, (ulong)&_end - 1);
#endif

    kprintf("%10d bytes stack space.\r\n", (ulong)memheap - (ulong)&_end);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (ulong)&_end, (ulong)memheap - 1);
#endif

    kprintf("%10d bytes heap space.\r\n",
            (ulong)platform.maxaddr - (ulong)memheap);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n\r\n",
            (ulong)memheap, (ulong)platform.maxaddr - 1);
#endif
    kprintf("\r\n");
}
