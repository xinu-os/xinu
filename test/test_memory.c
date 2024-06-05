#include <stddef.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>

/* function prototypes */
static bool list_check(void);
static void fatprocess(void);

/* test_memory -- allocates and frees memory; tests consistency of
 * memlist accounting.  Called by xsh_testsuite()
 */
thread test_memory(bool verbose)
{
    bool passed = TRUE;
    struct memblock *testblock;
    ulong *saddr;

    /* Allocate a small piece of memory */
    testPrint(verbose, "Allocate small piece");
    testblock = (struct memblock *)memget(1);
    if (SYSERR == (intptr_t)testblock)
    {
        passed = FALSE;
        testFail(verbose, "memget SYSERR");
    }
    else if (!list_check())
    {
        passed = FALSE;
        testFail(verbose,
                 "\nmemlist->length does not match computed free space");
    }
    else
    {
        testPass(verbose, "");
        /* and free the small piece of memory */
        testPrint(verbose, "Free small piece");
        if (SYSERR == memfree(testblock, 1))
        {
            passed = FALSE;
            testFail(verbose, "freemem SYSERR");
        }
        else if (!list_check())
        {
            passed = FALSE;
            testFail(verbose,
                     "\nmemlist->length does not match computed free space");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    /* Allocate all of memory, then free */
    testPrint(verbose, "Allocate 'all' of memory and free");
    fatprocess();
    if (!list_check())
    {
        passed = FALSE;
        testFail(verbose,
                 "\nmemlist->length does not match computed free space");
    }
    else
    {
        testPass(verbose, "");
    }

    saddr = stkget(1);
    if (SYSERR == (intptr_t)saddr)
    {
        passed = FALSE;
        testFail(verbose, "\nstkget SYSERR");
    }
    else
    {
        testPass(verbose, "");
        if (SYSERR == stkfree(saddr, 1))
        {
            passed = FALSE;
            testFail(verbose, "\nstkfree SYSERR");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    /* Final report */
    if (TRUE == passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}

/**
 * Computes size of free list by add length of each hop, and
 * compares this sum with the value maintained in memlist->length
 */
static bool list_check(void)
{
    struct memblock *mptr;
    int free = 0;

    for (mptr = memlist.next; mptr != NULL; mptr = mptr->next)
    {
        free += mptr->length;
    }

    if (memlist.length == free)
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * Eats all of the memory, then releases it
 */
static void fatprocess(void)
{
    struct fullblock
    {
        struct fullblock *fprev;
        unsigned int flen;
    };

    struct fullblock *fhead = NULL, *fnext = NULL;
    unsigned int size = 0, total = 0;

    size = 1 << 20;             // Size starts as 1 Megabyte
    total = 0;

    while (size > 8)
    {
        while ((struct fullblock *)SYSERR
               != (fnext = (struct fullblock *)memget(size)))
        {
            fnext->fprev = fhead;
            fnext->flen = size;
            fhead = fnext;
            total += size;
        }
        size = size >> 1;       // Try progressively smaller blocks
    }

    while (NULL != fhead)
    {
        fnext = fhead;
        fhead = fhead->fprev;
        memfree(fnext, fnext->flen);
    }
}
