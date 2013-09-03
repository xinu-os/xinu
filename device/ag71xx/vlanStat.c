/**
 * @file vlanStat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include "ag71xx.h"
#include <stdio.h>
#include <string.h>

int vlanStat(void)
{
    fprintf(stderr, "Switch Core not found!\n");
    return SYSERR;
}
