/**
 * @file backplaneCoreDisable.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <backplane.h>

#define	SBTML_RESET		0x1     /* reset */
#define	SBTML_REJ		0x2     /* reject */
#define	SBTML_CLK		0x10000 /* clock enable */
#define	SBTML_FGC		0x20000 /* force gated clocks on */
#define	SBTML_FL_MASK		0x3ffc0000      /* core-specific flags */
#define	SBTML_PE		0x40000000      /* pme enable */
#define	SBTML_BE		0x80000000      /* bist enable */

int waitOnBit(volatile ulong *, ulong, const ulong, ulong);

/**
 * Disable a Silicon Backplane core.
 * @param bpConfig pointer to device Silicon Backplane configuration space
 * @return OK if all goes well
 */
int backplaneCoreDisable(volatile struct backplaneConfig *bpConfig)
{
    volatile int i = 0;

    if (NULL == bpConfig)
    {
        return SYSERR;
    }

    /* Check if core is already disabled. */
    if (bpConfig->tmStateLow & SBTML_RESET)
    {
        return OK;
    }

    bpConfig->tmStateLow = (SBTML_REJ | SBTML_CLK);
    waitOnBit(&bpConfig->tmStateLow, SBTML_REJ, 1, 100000);
    waitOnBit(&bpConfig->tmStateHigh, SBTMH_BUSY, 0, 100000);
    bpConfig->tmStateLow =
        (SBTML_FGC | SBTML_CLK | SBTML_REJ | SBTML_RESET);
    bpConfig->tmStateLow;
    for (i = 100; i; i--)
        ;
    bpConfig->tmStateLow = (SBTML_REJ | SBTML_RESET);
    bpConfig->tmStateLow;
    for (i = 100; i; i--)
        ;
    return OK;
}
