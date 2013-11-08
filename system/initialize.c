/**
 * @file initialize.c
 * The system begins intializing after the C environment has been
 * established.  After intialization, the null thread remains always in
 * a ready (THRREADY) or running (THRCURR) state.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <kernel.h>
#include <backplane.h>
#include <clock.h>
#include <device.h>
#include <gpio.h>
#include <memory.h>
#include <bufpool.h>
#include <mips.h>
#include <thread.h>
#include <tlb.h>
#include <queue.h>
#include <semaphore.h>
#include <monitor.h>
#include <mailbox.h>
#include <network.h>
#include <nvram.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <safemem.h>
#include <platform.h>

#ifdef WITH_USB
#  include <usb_subsystem.h>
#endif

/* Function prototypes */
extern thread main(void);       /* main is the first thread created    */
static int sysinit(void);       /* intializes system structures        */

/* Declarations of major kernel variables */
struct thrent thrtab[NTHREAD];  /* Thread table                   */
struct sement semtab[NSEM];     /* Semaphore table                */
struct monent montab[NMON];     /* Monitor table                  */
qid_typ readylist;              /* List of READY threads          */
struct memblock memlist;        /* List of free memory blocks     */
struct bfpentry bfptab[NPOOL];  /* List of memory buffer pools    */

/* Active system status */
int thrcount;                   /* Number of live user threads         */
tid_typ thrcurrent;             /* Id of currently running thread      */

/* Params set by startup.S */
void *memheap;                  /* Bottom of heap (top of O/S stack)   */
ulong cpuid;                    /* Processor id                        */

struct platform platform;       /* Platform specific configuration     */

/**
 * Intializes the system and becomes the null thread.
 * This is where the system begins after the C environment has been 
 * established.  Interrupts are initially DISABLED, and must eventually 
 * be enabled explicitly.  This routine turns itself into the null thread 
 * after initialization.  Because the null thread must always remain ready 
 * to run, it cannot execute code that might cause it to be suspended, wait 
 * for a semaphore, or put to sleep, or exit.  In particular, it must not 
 * do I/O unless it uses kprintf for synchronous output.
 */
void nulluser(void)
{
    /* Platform-specific initialization  */
    platforminit();

    /* General initialization  */
    sysinit();

    /* Enable interrupts  */
    enable();

    /* Spawn the main thread  */
    ready(create(main, INITSTK, INITPRIO, "MAIN", 0), RESCHED_YES);

    /* null thread has nothing else to do but cannot exit  */
    while (TRUE)
    {
#ifndef DEBUG
        pause();
#endif                          /* DEBUG */
    }
}

/**
 * Intializes all Xinu data structures and devices.
 * @return OK if everything is initialized successfully
 */
static int sysinit(void)
{
    int i;
    struct thrent *thrptr;      /* thread control block pointer  */
    struct sement *semptr;      /* semaphore entry pointer       */
    struct monent *monptr;      /* monitor entry pointer         */
    struct memblock *pmblock;   /* memory block pointer          */
    struct bfpentry *bfpptr;

    /* Initialize system variables */
    /* Count this NULLTHREAD as the first thread in the system. */
    thrcount = 1;

    /* Initialize free memory list */
    memheap = roundmb(memheap);
    platform.maxaddr = truncmb(platform.maxaddr);
    memlist.next = pmblock = (struct memblock *)memheap;
    memlist.length = (uint)(platform.maxaddr - memheap);
    pmblock->next = NULL;
    pmblock->length = (uint)(platform.maxaddr - memheap);

    /* Initialize thread table */
    for (i = 0; i < NTHREAD; i++)
    {
        thrtab[i].state = THRFREE;
    }

    /* initialize null thread entry */
    thrptr = &thrtab[NULLTHREAD];
    thrptr->state = THRCURR;
    thrptr->prio = 0;
    strlcpy(thrptr->name, "prnull", TNMLEN);
    thrptr->stkbase = (void *)&_end;
    thrptr->stklen = (ulong)memheap - (ulong)&_end;
    thrptr->stkptr = 0;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;
    thrcurrent = NULLTHREAD;

    /* Initialize semaphores */
    for (i = 0; i < NSEM; i++)
    {
        semptr = &semtab[i];
        semptr->state = SFREE;
        semptr->count = 0;
        semptr->queue = queinit();
    }

    /* Initialize monitors */
    for (i = 0; i < NMON; i++)
    {
        monptr = &montab[i];
        monptr->state = SFREE;
        monptr->count = 0;
        monptr->owner = NOOWNER;
        monptr->sem = NULL;
    }

    /* Initialize buffer pools */
    for (i = 0; i < NPOOL; i++)
    {
        bfpptr = &bfptab[i];
        bfpptr->state = BFPFREE;
    }

    /* initialize thread ready list */
    readylist = queinit();

#if SB_BUS
    backplaneInit(NULL);
#endif                          /* SB_BUS */

#if RTCLOCK
    /* initialize real time clock */
    clkinit();
#endif                          /* RTCLOCK */

#ifdef UHEAP_SIZE
    /* Initialize user memory manager */
    {
        void *userheap;             /* pointer to user memory heap   */
        userheap = stkget(UHEAP_SIZE);
        if (SYSERR != (int)userheap)
        {
            userheap = (void *)((uint)userheap - UHEAP_SIZE + sizeof(int));
            memRegionInit(userheap, UHEAP_SIZE);

            /* initialize memory protection */
            safeInit();

            /* initialize kernel page mappings */
            safeKmapInit();
        }
    }
#endif

#if USE_TLB
    /* initialize TLB */
    tlbInit();
    /* register system call handler */
    exceptionVector[EXC_SYS] = syscall_entry;
#endif                          /* USE_TLB */

#if NMAILBOX
    /* intialize mailboxes */
    mailboxInit();
#endif

#if NDEVS
    for (i = 0; i < NDEVS; i++)
    {
        devtab[i].init((device*)&devtab[i]);
    }
#endif

#ifdef WITH_USB
    usbinit();
#endif

#if NVRAM
    nvramInit();
#endif

#if NNETIF
    netInit();
#endif

#if GPIO
    gpioLEDOn(GPIO_LED_CISCOWHT);
#endif
    return OK;
}
