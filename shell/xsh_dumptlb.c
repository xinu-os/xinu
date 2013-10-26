/**
 * @file     xsh_dumptlb.c
 * Dump the contents of the TLB.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <tlb.h>

#if USE_TLB
/**
 * @ingroup shell
 *
 * Shell command dumptlb prints all data in the TLB
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 */
shellcmd xsh_dumptlb(int nargs, char **args)
{
    /* check if user requests help */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays all the data in the TLB\n");
        printf("Options:\n");
        printf("\t--help\tdisplays this help message then exits\n");
        return 0;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for usage\n", args[0]);
        return 1;
    }
    if (nargs < 1)
    {
        fprintf(stderr, "%s: too few arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for usage\n", args[0]);
        return 1;
    }

    dumptlb();
    return 0;
}

void dumptlb(void)
{
    int i, asid;
    ulong entryhi = 0, entrylo0 = 0, entrylo1 = 0;

    /* grab current asid */
  asm("mfc0 %0, $10":"=r"(asid));
    asid &= 0x00ff;

    kprintf("Dumping TLB:\r\n");
    for (i = 0; i < TLB_SIZE; i++)
    {
        asm("mtc0 %0, $0;"      /* set index */
      :                        /* no output */
      :    "r"(i));
        asm("nop");
        asm("tlbr");            /* read TLB entry at index */
        asm("nop");
        asm("mfc0 %0, $2;"      /* save CP0_ENTRYLO0 */
      :    "=r"(entrylo0));
        asm("nop");
        asm("mfc0 %0, $3;"      /* save CP0_ENTRYLO1 */
      :    "=r"(entrylo1));
        asm("nop");
        asm("mfc0 %0, $10;"     /* save CP0_ENTRYHI */
      :    "=r"(entryhi));
        asm("nop");
        kprintf
            ("[%2d] entryhi: 0x%08X, entrylo0: 0x%08X, entrylo1: 0x%08X\r\n",
             i, entryhi, entrylo0, entrylo1);
    }

    /* reset to original asid */
  asm("mtc0 %0, $10": :"r"(asid));
}
#endif /* USE_TLB */
