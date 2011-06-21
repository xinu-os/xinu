/**
 * @file telnetInit.c
 * @provides telnetInit
 *
 * $Id: telnetInit.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <telnet.h>
#include <stdlib.h>

struct telnet telnettab[NTELNET];

/**
 * Initialize TELNET structures.
 * @param devptr TELNET device table entry
 * @return OK if device is initialized
 */
devcall telnetInit(device *devptr)
{
    struct telnet *tntptr;

    tntptr = &telnettab[devptr->minor];
    bzero(tntptr, sizeof(struct telnet));
    if (0 == devptr->minor)
    {
        tntptr->killswitch = semcreate(0);
    }
    else
    {
        tntptr->killswitch = telnettab[0].killswitch;
    }

    return OK;
}
