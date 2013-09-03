/**
 * @file rawInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <raw.h>
#include <stdlib.h>

struct raw rawtab[NRAW];

/**
 * @ingroup raw
 *
 * Initialize raw socket structures.
 * @param devptr RAW device table entry
 * @return OK if device is initialized
 */
devcall rawInit(device *devptr)
{
    struct raw *rawptr;

    rawptr = &rawtab[devptr->minor];
    bzero(rawptr, sizeof(struct raw));

    return OK;
}
