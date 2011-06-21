/**
 * @file backplaneCoreUp.c
 * @provides backplaneCoreUp.
 *
 * $Id: backplaneCoreUp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <backplane.h>

/**
 * Query Silicon Backplane status to determine if a core is "up".
 * @param bpConfig pointer to device Silicon Backplane configuration space
 * @return True if device core is up
 */
int backplaneCoreUp(volatile struct backplaneConfig *bpConfig)
{
    /* Core status can be determined by checking the target state       */
    /*  low register's Reset, Reject and Clock bits.                    */
    /*  Only 'Clock' should be set.                                     */
    if (NULL == bpConfig)
    {
        return SYSERR;
    }
    return (SBTML_CLK ==
            (bpConfig->tmStateLow &
             (SBTML_RESET | SBTML_REJ | SBTML_CLK)));
}
