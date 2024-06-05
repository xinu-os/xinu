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


#ifdef DETAIL
extern ulong cpuid;
#endif

#if __LP64__
#define HEXWIDTH 16
#define DECWIDTH 20
#else
#define HEXWIDTH 8
#define DECWIDTH 10
#endif

/* Start of kernel in memory (provided by linker)  */
extern void _start(void);

static void print_os_info(void)
{
    kprintf(VERSION);
    kprintf("\r\n\r\n");

#ifdef DETAIL
    /* Output detected platform. */
    kprintf("Processor identification: 0x%lX\r\n", cpuid);
    kprintf("Detected platform as: %s, %s\r\n\r\n",
            platform.family, platform.name);
#endif

    /* Output Xinu memory layout */
    kprintf("%*ld bytes physical memory.\r\n",
	    DECWIDTH, (uintptr_t)platform.maxaddr - (uintptr_t)platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%0*X to 0x%0*X]\r\n",
	    HEXWIDTH, (uintptr_t)platform.minaddr,
	    HEXWIDTH, (uintptr_t)(platform.maxaddr - 1));
#endif

    kprintf("%*ld bytes reserved system area.\r\n",
	    DECWIDTH, (uintptr_t)_start - (uintptr_t)platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%0*lX to 0x%0*lX]\r\n", 
	    HEXWIDTH, (uintptr_t)platform.minaddr,
	    HEXWIDTH, (uintptr_t)_start - 1);
#endif

    kprintf("%*ld bytes Xinu code.\r\n", DECWIDTH, (uintptr_t)&_etext - (uintptr_t)_start);
#ifdef DETAIL
    kprintf("           [0x%0*lX to 0x%0*lX]\r\n",
	    HEXWIDTH, (uintptr_t)_start,
	    HEXWIDTH, (uintptr_t)&_end - 1);
#endif

    kprintf("%*ld bytes stack space.\r\n",
	    DECWIDTH, (uintptr_t)memheap - (uintptr_t)&_end);
#ifdef DETAIL
    kprintf("           [0x%0*lX to 0x%0*lX]\r\n",
	    HEXWIDTH, (uintptr_t)&_end,
	    HEXWIDTH, (uintptr_t)memheap - 1);
#endif

    kprintf("%*ld bytes heap space.\r\n",
            DECWIDTH, (uintptr_t)platform.maxaddr - (uintptr_t)memheap);
#ifdef DETAIL
    kprintf("           [0x%0*lX to 0x%0*lX]\r\n\r\n",
	    HEXWIDTH, (uintptr_t)memheap,
	    HEXWIDTH, (uintptr_t)platform.maxaddr - 1);
#endif
    kprintf("\r\n");
}
