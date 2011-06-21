#include <stddef.h>
#include <safemem.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>

/* function prototypes */
static bool list_check(void);
static void fatprocess(void);

/* test_umemory -- allocates and frees memory; tests consistency of
 * memlist accounting.  Called by xsh_testsuite()
 */
thread test_umemory(bool verbose)
{
    bool passed = TRUE;
    void *memptr;

    /* Allocate no memory */
    testPrint(verbose, "malloc(0)");
    memptr = malloc(0);
    failif(NULL != memptr, "Non-NULL pointer detected");

    /* Allocate major memory */
    testPrint(verbose, "malloc(2147483647)");
    memptr = malloc(2147483647);
    failif(NULL != memptr, "Too much memory.");

    /* Allocate a small piece of memory */
    testPrint(verbose, "Allocate small piece");
    memptr = (void *)malloc(1);
    if (NULL == memptr)
    {
        passed = FALSE;
        testFail(verbose, "malloc NULL");
    }
    else if (!list_check())
    {
        passed = FALSE;
        testFail(verbose, "\nlength does not match computed free space");
    }
    else
    {
        testPass(verbose, "");
        /* and free the small piece of memory */
        testPrint(verbose, "Free small piece");
        free(memptr);
        if (!list_check())
        {
            passed = FALSE;
            testFail(verbose,
                     "\nlength does not match computed free space");
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
        testFail(verbose, "\nlength does not match computed free space");
    }
    else
    {
        testPass(verbose, "");
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
 * compares this sum with the value maintained in thread->memlist.length
 */
static bool list_check(void)
{
    struct thrent *thread;
    struct memblock *mptr;
    int free = 0;

    thread = &(thrtab[gettid()]);

    for (mptr = thread->memlist.next; mptr != NULL; mptr = mptr->next)
    {
        free += mptr->length;
    }

    if (thread->memlist.length == free)
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
        while ((struct fullblock *)NULL
               != (fnext = (struct fullblock *)malloc(size)))
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
        free(fnext);
    }
}
