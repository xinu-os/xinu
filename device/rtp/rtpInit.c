/**
 * @file rtpInit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdlib.h>
#include <rtp.h>
#include <udp.h>

struct rtp rtptab[NRTP];

/**
 * Set aside some space for a RTP device to be opened on
 * @param devptr UDP device table entry
 * @return OK
 */
devcall rtpInit(device *devptr)
{
    struct rtp *rtpptr;

    rtpptr = &rtptab[devptr->minor];
    bzero(rtpptr, sizeof(struct rtp));

    return OK;
}
