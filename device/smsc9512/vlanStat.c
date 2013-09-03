/**
 * @file vlanStat.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <ether.h>
#include <stdio.h>

int vlanStat(void)
{
    fprintf(stderr, "ERROR: VLANs not supported by this driver.\n");
    return SYSERR;
}
