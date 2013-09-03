/**
 * @file     udpOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

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
 * @ingroup udpexternal
 *
 * Associate a UDP socket with local and remote IP addresses and ports, and
 * prepare it for receiving and sending data with udpRead() and udpWrite().
 *
 * @param devptr
 *      Device table entry for the UDP device.
 *
 * @param ap Four additional arguments, specifying the following in order:
 *     - The local IP address.
 *     - The remote IP address.  May be @c NULL to create an initially
 *       unbound socket.
 *     - The local port.  May be 0 to auto-assign a port number.
 *     - The remote port.  May be 0 if creating an initially unbound socket.
 *
 * @return ::OK if the UDP device was opened successfully; otherwise ::SYSERR.
 */
devcall udpOpen(device *devptr, va_list ap)
{
    irqmask im;
    int retval;
    struct udp *udpptr;
    const struct netaddr *localip;
    const struct netaddr *remoteip;
    ushort localpt;
    ushort remotept;

    udpptr = &udptab[devptr->minor];

    im = disable();
    /* Check if UDP is already open */
    if (UDP_OPEN == udpptr->state)
    {
        UDP_TRACE("udp%d has already been opened.", devptr->minor);
        retval = SYSERR;
        goto out_restore;
    }

    udpptr->state = UDP_OPEN;
    udpptr->dev = devptr;

    /* Initialize incoming packet buffer */
    udpptr->icount = 0;
    udpptr->istart = 0;

    /* Initialize the semaphore */
    udpptr->isem = semcreate(0);

    if (SYSERR == (int)udpptr->isem)
    {
        retval = SYSERR;
        goto out_udp_close;
    }

    /* Retrieve port and address arguments */
    localip = va_arg(ap, const struct netaddr *);
    remoteip = va_arg(ap, const struct netaddr *);
    localpt = va_arg(ap, int);
    remotept = va_arg(ap, int);

    /* Initialize ports and addresses */

    /* Local IP address is required */
    if (NULL == localip)
    {
        retval = SYSERR;
        goto out_free_sem;
    }

    netaddrcpy(&udpptr->localip, localip);

    /* Remote IP address is not required */
    if (NULL == remoteip)
    {
        bzero(&udpptr->remoteip, sizeof(struct netaddr));
    }
    else
    {
        netaddrcpy(&udpptr->remoteip, remoteip);
    }

    /* Allocate a local port if none is specified */
    if (0 == localpt)
    {
        localpt = allocPort();
    }

    udpptr->localpt = localpt;
    udpptr->remotept = remotept;

    /* Allocate received UDP packet buffer pool */
    udpptr->inPool = bfpalloc(NET_MAX_PKTLEN, UDP_MAX_PKTS);
    if (SYSERR == (int)udpptr->inPool)
    {
        retval = SYSERR;
        goto out_release_port;
    }
    UDP_TRACE("udp%d inPool has been assigned pool ID %d.\r\n",
              devptr->minor, udpptr->inPool);

    udpptr->flags = 0;

    retval = OK;
    goto out_restore;

out_release_port:
    udpptr->localpt = 0;
out_free_sem:
    semfree(udpptr->isem);
out_udp_close:
    udpptr->state = UDP_FREE;
out_restore:
    restore(im);
    return retval;
}

/**
 * Allocates an unused UDP port to use as a local port
 * @return UDP port
 */
static ushort allocPort(void)
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
