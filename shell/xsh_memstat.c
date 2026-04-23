/**
 * @file     xsh_memstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <platform.h>
#include <mips.h>
#include <memory.h>
#include <safemem.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>
#include <stdlib.h>

#define PRINT_DEFAULT 0x01
#define PRINT_KERNEL  0x02
#define PRINT_REGION  0x04
#define PRINT_THREAD  0x08

#if  __LP64__
#define HEXWIDTH 16
#else
#define HEXWIDTH 8
#endif

extern char *maxaddr;
extern void _start(void);

static void printMemUsage(void);
static void printRegAllocList(void);
static void printRegFreeList(void);
static void printFreeList(struct memblock *, char *);

static void usage(char *command)
{
    printf("Usage: %s [-r] [-k] [-q] [-t <TID>]\n\n", command);
    printf("Description:\n");
    printf("\tDisplays the current memory usage and prints the\n");
    printf("\tfree list.\n");
    printf("Options:\n");
    printf("\t-r\t\tprint region allocated and free lists\n");
    printf("\t-k\t\tprint kernel free list\n");
    printf("\t-q\t\tsuppress current system memory usage screen\n");
    printf("\t-t <TID>\tprint user free list of thread id tid\n");
    printf("\t--help\t\tdisplay this help and exit\n");
}



/**
 * @ingroup shell
 *
 * Shell command (gpiostat) provides memory use and free list information.
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_memstat(int nargs, char *args[])
{
    int i;
    tid_typ tid;                /* thread to dump memlist of      */
    char print;                 /* print region free/alloc lists  */

    print = PRINT_DEFAULT;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        usage(args[0]);
        return 0;
    }

    tid = BADTID;
    for (i = 1; i < nargs; i++)
    {
        if (0 == strcmp(args[i], "-t"))
        {
            if (i + 1 < nargs)
            {
                /* Get thread id to display freelist of */
                print |= PRINT_THREAD;
                tid = atoi(args[i + 1]);
            }
            else
            {
                usage(args[0]);
                return 1;
            }
        }
        else if (0 == strcmp(args[i], "-r"))
        {
            print |= PRINT_REGION;
        }
        else if (0 == strcmp(args[i], "-k"))
        {
            print |= PRINT_KERNEL;
        }
        else if (0 == strcmp(args[i], "-q"))
        {
            print &= ~(PRINT_DEFAULT);
        }
    }

    if (print & PRINT_DEFAULT)
    {
        printMemUsage();
    }

    if (print & PRINT_REGION)
    {
        printRegAllocList();
        printRegFreeList();
    }

    if (print & PRINT_KERNEL)
    {
        printFreeList(&memlist, "kernel");
    }

    if (print & PRINT_THREAD)
    {
        if (isbadtid(tid))
        {
            fprintf(stderr, "Invalid thread id: %d\n", tid);
        }
        else
        {
            printFreeList(&(thrtab[tid].memlist), thrtab[tid].name);
        }
    }

    return 0;
}

static void printMemUsage(void)
{
    int i;
    uintptr_t phys = 0;         /* total physical memory          */
    uintptr_t resrv = 0;        /* total reserved system memory   */
    uintptr_t code = 0;         /* total Xinu code memory         */
    uintptr_t stack = 0;        /* total stack memory             */
    uintptr_t kheap = 0;        /* total kernel heap memory       */
    uintptr_t kused = 0;        /* total used kernel heap memory  */
    uintptr_t kfree = 0;        /* total free memory              */
    struct memblock *block;     /* memory block pointer           */
#ifdef UHEAP_SIZE
    uintptr_t uheap = 0;        /* total user heap memory         */
    uintptr_t uused = 0;        /* total used user heap memory    */
    struct memregion *regptr;   /* point to memory region */
#endif                          /* UHEAP_SIZE */


#ifdef _XINU_ARCH_MIPS_
    /* Calculate amount of physical memory */
    phys = (ulong)platform.maxaddr - (ulong)KSEG0_BASE;
    /* Calculate amount of reserved system memory */
    resrv = (ulong)_start - (ulong)KSEG0_BASE;
#else
    phys = (ulong)platform.maxaddr - (ulong)platform.minaddr;
    resrv = (ulong)_start - (ulong)platform.minaddr;
#endif
    
    /* Calculate amount of text memory */
    code = (ulong)&_end - (ulong)_start;

    /* Caculate amount of stack memory */
    for (i = 0; i < NTHREAD; i++)
    {
        if (thrtab[i].state != THRFREE)
        {
            stack += (ulong)thrtab[i].stklen;
        }
    }

    /* Calculate amount of free kernel memory */
    for (block = memlist.next; block != NULL; block = block->next)
    {
        kfree += block->length;
    }

    /* Caculate amount of kernel heap memory */
    kheap = phys - resrv - code - stack;
    kused = kheap - kfree;

    /* Calculate amount of user heap memory */
#ifdef UHEAP_SIZE
    /* determine used user heap amount */
    for (regptr = regalloclist; (uintptr_t)regptr != SYSERR;
         regptr = regptr->next)
    {
        uused += regptr->length;
    }

    /* determine total user heap size */
    uheap = uused;
    for (regptr = regfreelist; (uintptr_t)regptr != SYSERR;
         regptr = regptr->next)
    {
        uheap += regptr->length;
    }

    /* the kernel donates used memory, so update those vars */
    kheap -= uheap;
    kused -= uheap;
#endif                          /* UHEAP_SIZE */

    /* Ouput current memory usage */
    printf("Current System Memory Usage:\n");
    printf("-----------------------------------------------------\n");
    printf("%20ld bytes system area\n", (ulong)resrv);
    printf("%20ld bytes Xinu code\n",  (ulong)code);
    printf("%20ld bytes stack space\n",  (ulong)stack);
    printf("%20ld bytes kernel heap space (%ld used)\n",  (ulong)kheap,  (ulong)kused);
#ifdef UHEAP_SIZE
    printf("%20ld bytes user heap space (%ld used)\n",  (ulong)uheap,  (ulong)uused);
#endif                          /* UHEAP_SIZE */
    printf("-----------------------------------------------------\n");
    printf("%20ld bytes physical memory\n\n", (ulong)phys);
}

/**
 * Dump the current contents of the allocated region list.
 */
static void printRegAllocList(void)
{
#ifdef UHEAP_SIZE
    uint index;
    struct memregion *regptr;

    /* Output free list */
    printf("Region Allocated List:\n");
    printf("Index  Start                Length               TID\n");
    printf("-----  -------------------  -------------------  ---\n");

    for (regptr = regalloclist; (uintptr_t)regptr != SYSERR;
         regptr = regptr->next)
    {
        index = ((uintptr_t)regptr - (uintptr_t)regtab) / sizeof(struct memregion);
        printf("%5d  0x%*lx  %20ld  %3d\n", index, HEXWIDTH, (ulong)regptr->start,
               (ulong)regptr->length, regptr->thread_id);
    }
    printf("\n");
#else
    fprintf(stderr, "No user heap available.\n\n");
#endif                          /* UHEAP_SIZE */
}

/**
 * Dump the current contents of the free region list.
 */
static void printRegFreeList(void)
{
#ifdef UHEAP_SIZE
    uint index;
    struct memregion *regptr;

    /* Output free list */
    printf("Region Free List:\n");
    printf("Index  Start            Length\n");
    printf("-----  ---------------  -----------------\n");

    for (regptr = regfreelist; (int)regptr != SYSERR;
         regptr = regptr->next)
    {
        index = ((uintptr_t)regptr - (uintptr_t)regtab) / sizeof(struct memregion);
        printf("%5d  0x%0*lx  %20ld\n", index, HEXWIDTH, (ulong)regptr->start,
               (ulong)regptr->length);
    }
    printf("\n");
#endif                          /* UHEAP_SIZE */
}


/**
 * Dump the current free list of a specific thread.
 * @param tid Id of thread to dump free list.
 */
static void printFreeList(struct memblock *base, char *ident)
{
    struct memblock *block;

    /* Output free list */
    printf("Free List (%s):\n", ident);
    printf("BLOCK START         LENGTH  \n");
    printf("------------------  --------------------\n");
    for (block = base->next; block != NULL; block = block->next)
    {
        printf("0x%0*lX  %20lu\n", HEXWIDTH, (ulong)block, (ulong)block->length);
    }
    printf("\n");
}
