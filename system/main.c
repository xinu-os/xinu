/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <device.h>
#include <platform.h>
#include <shell.h>
#include <stdio.h>
#include <tee.h>
#include <thread.h>
#include <version.h>

static void print_os_info(void);
static int open_console(void);
static int start_shells(void);

/**
 * Main thread.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.
 */
thread main(void)
{
    /* Print information about the operating system.  */
    print_os_info();

#ifdef CONSOLE
    /* Associate the CONSOLE with the main input and output devices  */
    if (SYSERR == open_console())
    {
        kprintf("WARNING: Can't open CONSOLE device\r\n");
    }
#endif

#if defined(TTY1) && defined(SERIAL1)
    /* Associate TTY1 with second serial port, if present.  */
    if (SYSERR == open(TTY1, SERIAL1))
    {
        kprintf("WARNING: Can't open TTY1 over SERIAL1\r\n");
    }
#endif

#if NETHER
    /* Open all ethernet devices */
    {
        int dev;

        for (dev = ETH0; dev < ETH0 + NETHER; dev++)
        {
            if (SYSERR == open(dev))
            {
                kprintf("WARNING: Failed to open ETH%d\r\n", dev - ETH0);
            }
        }
    }
#endif

    /* If shell enabled, start one on each available TTY  */
#if HAVE_SHELL
    if (SYSERR == start_shells())
    {
        kprintf("WARNING: Failed to start all shells\n");
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

    kprintf("%10d bytes Xinu code.\r\n", (ulong)&_end - (ulong)_start);
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

#ifdef CONSOLE
/* Attempt to open CONSOLE, which is the default TTY device.  Default to the
 * first UART, unless there is a TEE device available, in which case input can
 * also be taken from a USB keyboard (if available) and output can be cloned to
 * a framebuffer (if available).  */
static int open_console(void)
{
    int maindev = -1;

#ifdef SERIAL0
    maindev = SERIAL0;
#endif

#if NTEE
    {
        int ndevs = 0;
    #ifdef SERIAL0
        ndevs++;
    #endif
    #ifdef FRAMEBUF
        ndevs++;
    #endif
    #ifdef USBKBD0
        ndevs++;
    #endif

        if (ndevs >= 2)
        {
            int teedev = teeAlloc();
            if (SYSERR != teedev)
            {
                if (SYSERR != open(teedev, ndevs
                            #ifdef SERIAL0
                                   ,SERIAL0
                            #endif
                            #ifdef FRAMEBUF
                                   ,FRAMEBUF
                            #endif
                            #ifdef USBKBD0
                                   ,USBKBD0
                            #endif
                                  ))
                {
                    maindev = teedev;
                #ifdef FRAMEBUF
                    /* Framebuffer is output-only  */
                    control(teedev, TEE_CTRL_SUBDEV_SET_FLAGS, FRAMEBUF,
                            TEE_SUBDEV_FLAG_READ_DISABLED);
                #endif
                #ifdef USBKBD0
                    /* Keyboard is input-only  */
                    control(teedev, TEE_CTRL_SUBDEV_SET_FLAGS, USBKBD0,
                            TEE_SUBDEV_FLAG_WRITE_DISABLED);
                #endif
                    /* Don't let write errors to one device (e.g. FRAMEBUF) mask
                     * successful writes to another (e.g. SERIAL0).  */
                    control(teedev, TEE_CTRL_SET_FLAGS, TEE_FLAG_BEST_WRITE, 0);
                }
            }
        }
    }
#endif /* NTEE */

    return open(CONSOLE, maindev);
}
#endif /* CONSOLE */

#ifdef HAVE_SHELL

/* Start a shell on each opened TTY device  */
static int start_shells(void)
{
    int ttys[NTTY];
    int nttys = 0;
    int retval = OK;
    int i;

#ifdef CONSOLE
    ttys[nttys++] = CONSOLE;
#endif

#ifdef TTY1
    ttys[nttys++] = TTY1;
#endif

    for (i = 0; i < nttys; i++)
    {
        char name[TNMLEN];
        sprintf(name, "SHELL%d", i);
        if (SYSERR == ready(create
                            (shell, INITSTK, INITPRIO, name, 3,
                             ttys[i], ttys[i], ttys[i]), RESCHED_NO))
        {
            retval = SYSERR;
        }
    }
    return retval;
}
#endif /* HAVE_SHELL */
