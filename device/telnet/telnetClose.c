/**
 * @file telnetClose.c
 * @provides telnetClose.
 *
 * $Id: telnetClose.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <telnet.h>
#include <stdlib.h>

/* Close a TELNET device.
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
