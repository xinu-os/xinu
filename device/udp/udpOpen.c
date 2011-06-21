/**
 * @file     udpOpen.c
 * @provides udpOpen
 *
 * $Id: udpOpen.c 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <bufpool.h>
#include <device.h>
#include <network.h>
#include <udp.h>
#include <stdarg.h>
#include <interrupt.h>

static ushort allocPort(void);

/**
 * Associate a UDP socket with a hardware device.
 * @param devptr UDP device table entry
 * @param ap 2nd argument is the local IP address
 *           3rd argument is the remote IP address
 *           4th argument is the local port (auto-assigned if zero)
 *           5th argument is the remote port
 * @return OK if UDP is opened properly, otherwise SYSERR
 */
devcall udpOpen(device *devptr, va_list ap)
{
    struct udp *udpptr = NULL;
    ushort localpt = 0;
    ushort remotept = 0;
    struct netaddr *localip = NULL;
    struct netaddr *remoteip = NULL;
    irqmask im;

    udpptr = &udptab[devptr->minor];

    im = disable();
    /* Check if UDP is already open */
    if (UDP_OPEN == udpptr->state)
    {
        UDP_TRACE("udp%d has already been opened.", devptr->minor);
        restore(im);
        return SYSERR;
    }

    udpptr->state = UDP_OPEN;
    udpptr->dev = devptr;

    /* Initialize incoming packet buffer */
    udpptr->icount = 0;
    udpptr->istart = 0;

    /* Initialize the semaphore */
    udpptr->isem = semcreate(0);

    /* Parse arguments */
    localip = va_arg(ap, struct netaddr *);
    remoteip = va_arg(ap, struct netaddr *);
    localpt = va_arg(ap, int);
    remotept = va_arg(ap, int);

    /* Allocate a local port if none is specified */
    if (NULL == localpt)
    {
        localpt = allocPort();
    }

    /* Set remote port to 0 if none is specified */
    if (NULL == remotept)
    {
        remotept = 0;
    }

    /* Local IP is required */
    if (NULL == localip)
    {
        restore(im);
        return SYSERR;
    }

    if (NULL == remoteip)
    {
        bzero(&(udpptr->remoteip), sizeof(struct netaddr));
    }

    /* Initialize ports and addresses */
    udpptr->localpt = (ushort)localpt;
    udpptr->remotept = (ushort)remotept;

    if (NULL != localip)
    {
        netaddrcpy(&(udpptr->localip), localip);
    }

    if (NULL != remoteip)
    {
        netaddrcpy(&(udpptr->remoteip), remoteip);
    }

    /* Allocate received UDP packet buffer pool */
    udpptr->inPool = bfpalloc(NET_MAX_PKTLEN, UDP_MAX_PKTS);
    UDP_TRACE("udp%d inPool has been assigned pool ID %d.\r\n",
              devptr->minor, udpptr->inPool);

    udpptr->flags = 0;

    restore(im);
    return OK;
}

/**
 * Allocates an unused UDP port to use as a local port
 * @return UDP port
 */
static ushort allocPort()
{
    static ushort nextport = 0;
    int i = 0;

    nextport = (nextport + 1) % (UDP_PMAX - UDP_PSTART);
    for (i = 0; i < NUDP; i++)
    {
        if ((udptab[i].localpt == (nextport + UDP_PSTART))
            || (UDP_PORT_TRACEROUTE == (nextport + UDP_PSTART)))
        {
            nextport = (nextport + 1) % (UDP_PMAX - UDP_PSTART);
            i = 0;
        }
    }

    return (nextport + UDP_PSTART);
}
