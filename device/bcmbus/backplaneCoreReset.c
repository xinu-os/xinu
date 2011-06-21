/**
 * @file backplaneCoreReset.c
 * @provides backplaneReset.
 *
 * $Id: backplaneCoreReset.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <backplane.h>

int backplaneReset(volatile struct backplaneConfig *bpConfig)
{
    volatile int i = 0;

    /* Reset the target core. */
    backplaneCoreDisable(bpConfig);
    bpConfig->tmStateLow = (SBTML_RESET | SBTML_CLK | SBTML_FGC);
    bpConfig->tmStateLow;
    for (i = 10; i; i--)
        ;
    /* Clear SERR if set, this is a hw bug workaround.  */
    if (bpConfig->tmStateHigh & SBTMH_SERR)
    {
        bpConfig->tmStateHigh = 0;
    }
    /* Clear any state errors. */
    i = bpConfig->imState;
    if (i & (SBIM_IBE | SBIM_TO))
    {
        bpConfig->imState = i & ~(SBIM_IBE | SBIM_TO);
    }
    bpConfig->tmStateLow = (SBTML_CLK | SBTML_FGC);
    bpConfig->tmStateLow;
    for (i = 10; i; i--)
        ;
    bpConfig->tmStateLow = SBTML_CLK;
    bpConfig->tmStateLow;
    for (i = 10; i; i--)
        ;
    return OK;
}
