/**
 * @file telnetClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <telnet.h>
#include <stdlib.h>

/**
 * @ingroup telnet
 *
 * Close a TELNET device.
 * @param devptr TELNET device table entry
 * @return OK if TELNET is closed properly, otherwise SYSERR
 */
devcall telnetClose(device *devptr)
{
    struct telnet *tntptr;

    tntptr = &telnettab[devptr->minor];
    bzero(tntptr, sizeof(struct telnet));
    tntptr->state = TELNET_STATE_FREE;
    return OK;
}
