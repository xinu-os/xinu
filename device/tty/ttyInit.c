/**
 * @file ttyInit.c
 * @provides ttyInit.
 *
 * $Id: ttyInit.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <tty.h>
#include <stdlib.h>

struct tty ttytab[NTTY];

/**
 * Initialize TTY structures.
 * @param devptr TTY device table entry
 * @return OK if device is initialized
 */
devcall ttyInit(device *devptr)
{
    struct tty *ttyptr;

    ttyptr = &ttytab[devptr->minor];
    bzero(ttyptr, sizeof(struct tty));

    return OK;
}
