/**
 * @file rawOpen.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <network.h>
#include <raw.h>
#include <stdarg.h>
#include <interrupt.h>

/**
 * @ingroup raw
 *
 * Open a raw socket.
 * @param devptr RAW device table entry
 * @param ap 2nd argument is the local IP address
 *           3rd argument is the remote IP address
 *           4th argument is the IP protocol
 * @return OK if raw socket is opened properly, otherwise SYSERR
 */
devcall rawOpen(device *devptr, va_list ap)
{
    struct raw *rawptr = NULL;
    ushort proto;
    struct netaddr *localip;
    struct netaddr *remoteip;
    irqmask im;

    /* Setup pointer to raw socket control block */
    rawptr = &rawtab[devptr->minor];

    /* Obtain arguments */
    localip = (struct netaddr *)va_arg(ap, int);
    remoteip = (struct netaddr *)va_arg(ap, int);
    proto = va_arg(ap, int);

    im = disable();
    /* Check if raw socket is already open */
    if (RAW_FREE != rawptr->state)
    {
        restore(im);
        return SYSERR;
    }

    rawptr->state = RAW_ALLOC;
    /* Link raw control block with device table entry */
    rawptr->dev = devptr;

    /* Initialize connection fields */
    rawptr->proto = proto;
    if (NULL == localip)
    {
        rawptr->localip.type = NULL;
    }
    else
    {
        netaddrcpy(&rawptr->localip, localip);
    }
    if (NULL == remoteip)
    {
        rawptr->remoteip.type = NULL;
    }
    else
    {
        netaddrcpy(&rawptr->remoteip, remoteip);
    }

    /* Initialize input */
    rawptr->istart = 0;
    rawptr->icount = 0;
    rawptr->isema = semcreate(0);
    if (SYSERR == (int)rawptr->isema)
    {
        restore(im);
        return SYSERR;
    }
    rawptr->flags = NULL;

    restore(im);
    return OK;
}
