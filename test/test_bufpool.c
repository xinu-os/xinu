#include <stddef.h>
#include <memory.h>
#include <bufpool.h>
#include <stdio.h>
#include <string.h>
#include <testsuite.h>

#define TBUFSIZE  32
#define TBUFNUM   32

thread test_bufpool(bool verbose)
{
    bool passed = TRUE;
    int id, i;
    void *pbuf;
    void *chain[TBUFNUM];
    irqmask im;
    ulong memsize;
    char datums[] = { "abcdefghijklmnopqrstuvwxyz1234567" };

    /* Create buffer pool */
    testPrint(verbose, "Create buffer pool");
    im = disable();
    memsize = memlist.length;
    id = bfpalloc(TBUFSIZE, TBUFNUM);
    if (memlist.length !=
        memsize - ((TBUFSIZE + sizeof(struct poolbuf)) * TBUFNUM))
    {
        restore(im);
        testFail(verbose,
                 "\nmemlist.length reduction does not match pool size");
        return OK;
    }
    else
    {
        restore(im);
        testPass(verbose, "");
    }

    /* Allocate single buffer */
    testPrint(verbose, "Allocate single buffer");
    pbuf = bufget(id);
    if (SYSERR == (ulong)pbuf)
    {
        passed = FALSE;
        testFail(verbose, "\nbufget() returns SYSERR");
    }
    else
    {
        testPass(verbose, "");
        /* Return single buffer */
        testPrint(verbose, "Return single buffer");
        if (SYSERR == buffree(pbuf))
        {
            passed = FALSE;
            testFail(verbose, "\nbuffree() returns SYSERR");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    /* Allocate and fill all buffers in pool */
    testPrint(verbose, "Allocate and fill all buffers");
    for (i = 0; i < TBUFNUM; i++)
    {
        pbuf = bufget(id);
        chain[i] = pbuf;
        if (SYSERR == (ulong)pbuf)
        {
            break;
        }
        memcpy(pbuf, datums, TBUFSIZE);
    }
    if (TBUFNUM != i)
    {
        passed = FALSE;
        testFail(verbose, "\ngetbuf() returns SYSERR");
    }
    else
    {
        for (i = 0; i < TBUFNUM; i++)
        {
            pbuf = chain[i];
            if (0 != memcmp(pbuf, datums, TBUFSIZE))
            {
                break;
            }
        }
        if (TBUFNUM != i)
        {
            passed = FALSE;
            testFail(verbose, "\nbuffer data does not match source");
        }
        else
        {
            testPass(verbose, "");
            /* Free all buffers in pool */
            testPrint(verbose, "Free all buffers");
            for (i = 0; i < TBUFNUM; i++)
            {
                pbuf = chain[i];
                if (SYSERR == buffree(pbuf))
                {
                    break;
                }
            }
            if (TBUFNUM != i)
            {
                passed = FALSE;
                testFail(verbose, "\nbuffree() returns SYSERR");
            }
            else
            {
                testPass(verbose, "");
            }
        }
    }

    /* Release pool */
    testPrint(verbose, "Free buffer pool");
    im = disable();
    memsize = memlist.length;
    bfpfree(id);
    if (memlist.length !=
        memsize + ((TBUFSIZE + sizeof(struct poolbuf)) * TBUFNUM))
    {
        restore(im);
        passed = FALSE;
        testFail(verbose,
                 "\nmemlist.length increase does not match pool size");
    }
    else
    {
        restore(im);
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
