/**
 * @file     tlbInit.c
 * Initialize the Translation lookaside buffer.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <tlb.h>
#include <string.h>

interrupt tlbMiss(void);
interrupt tlbMissLong(void);

/**
 * Initialize the TLB.  This function is called at startup.  Installs
 * handler for both TLB load and store operations in normal exceptionVector
 * table, also copies the handler to the quick tlbMiss memory 0x80000000.
 */
void tlbInit(void)
{
    /* Register slow TLB exception handler (KSEG2 and USEG misses) */
    exceptionVector[EXC_TLBS] = (void *)tlbMissLong;
    exceptionVector[EXC_TLBL] = (void *)tlbMissLong;

    /* install the quick handler (for USEG mappings) */
    memcpy(TLB_EXC_START, tlbMiss, TLB_EXC_LENGTH);
}
