#include <stdio.h>
#include <stddef.h>
#include <testsuite.h>
#include <mips.h>
#include <safemem.h>
#include <stdlib.h>

#if USE_TLB

/* Known values to use */
#define TEST_LENGTH (4*PAGE_SIZE)

#define EVN_DATA1   0x12345678
#define ODD_DATA1   0xfedcba98
#define EVN_DATA2   0x4d696b65
#define ODD_DATA2   0x656b694d
#define FAULT_DATA  0x58696e75

/**
 * Attempt to use an address originating from a different process.  Should
 * cause a memory protection violation and die.
 */
thread tlb_violator(int *address)
{
    /* conceal error output */
    thrtab[thrcurrent].fdesc[2] = CONSOLE;

    *address = FAULT_DATA;

    /* Don't do anything */
    while (TRUE)
        ;
    return OK;
}

#endif

/**
 * Testsuite to make sure the TLB is working "as expected."  By "as
 * expected," this means that the mapping is 1:1 between kernel and user
 * space.
 */
thread test_tlb(bool verbose)
{
#if USE_TLB
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;

    int kseg_data[2];           /* data as stored in KSEG1 */
    int user_data[2];           /* data as stored in KUSEG */
    int *ptr[2], *kseg_ptr;     /* memory pointers         */
    tid_typ id_vio;             /* thread id of violator   */

    /* grab enough memory to cause two faults */
    kseg_ptr = (int *)malloc(TEST_LENGTH);
    if (kseg_ptr == NULL)
    {
        testSkip(TRUE, "could not malloc");
        return 1;
    }

    /* bump ptr to kseg1 (unmapped, uncached) */
    ptr[0] = (int *)((uint)kseg_ptr | KSEG1_BASE);

    /* Set up pointers for an even and odd page */
    ptr[1] = (int *)((uint)ptr[0] + (3 * PAGE_SIZE));

    /* Set data to known values and store them for comparison */
    *(ptr[0]) = kseg_data[0] = EVN_DATA1;
    *(ptr[1]) = kseg_data[1] = ODD_DATA1;

    /* Test to make sure we read the correct values for USEG */
    testPrint(verbose, "Read known data from USEG");

    /* bump pointers down to user space */
    ptr[0] = (int *)((int)ptr[0] & PMEM_MASK);
    ptr[1] = (int *)((int)ptr[1] & PMEM_MASK);

    /* gather data, as seen by user space */
    user_data[0] = *(ptr[0]);
    user_data[1] = *(ptr[1]);

    failif((user_data[0] != kseg_data[0]) || (user_data[0] != EVN_DATA1)
           || (user_data[1] != kseg_data[1])
           || (user_data[1] != ODD_DATA1),
           "USEG and KSEG data does not match");

    /* This is the second "subtest" of this suite */
    testPrint(verbose, "Write known data to USEG");

    /* change to new known values */
    *(ptr[0]) = EVN_DATA2;
    *(ptr[1]) = ODD_DATA2;

    failif((EVN_DATA2 != *(ptr[0])) || (ODD_DATA2 != *(ptr[1])),
           "Could not write USEG data");

    /* attempt to cause a memory protection violation */
    testPrint(verbose, "Memory protection violation");
    ready(id_vio =
          create((void *)tlb_violator, INITSTK,
                 thrtab[thrcurrent].prio + 1, "TLB Violator", 1, ptr[0]),
          RESCHED_YES);

    /* if we get back here, the violator was killed */
    failif((EVN_DATA2 != *(ptr[0]) || (thrtab[id_vio].state != THRFREE)),
           "Violator wrote and ran");

    /* free the pages */
    free(kseg_ptr);

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

#else /* USE_TLB */
    testSkip(TRUE, "");
#endif /* !USE_TLB */
    return OK;
}
