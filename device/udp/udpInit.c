/**
 * @file udpInit.c
 * @provides udpInit
 *
 * $Id: udpInit.c 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdlib.h>
#include <udp.h>

struct udp udptab[NUDP];

/**
 * Set aside some space for a UDP device to be opened on
 * @param devptr UDP device table entry
 * @return OK
 */
devcall udpInit(device *devptr)
{
    struct udp *udpptr;

    udpptr = &udptab[devptr->minor];
    bzero(udpptr, sizeof(struct udp));

    return OK;
}
