/**
 * @file ethloopControl.c
 * @provides ethloopControl
 *
 * $Id: ethloopControl.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <bufpool.h>
#include <device.h>
#include <ethernet.h>
#include <ethloop.h>
#include <interrupt.h>
#include <network.h>
#include <stdlib.h>

/**
 * Control function for ethloop devices.
 * @param devptr ethloop device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
devcall ethloopControl(device *devptr, int func, long arg1, long arg2)
{
    struct ethloop *elpptr;
    struct netaddr *addr;
    uchar old;
    irqmask im;
    char *buf;
    char *hold;
    int holdlen;

    elpptr = &elooptab[devptr->minor];

    im = disable();
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
        restore(im);
        return SYSERR;
    }

    switch (func)
    {
/* Get link header length. */
    case NET_GET_LINKHDRLEN:
        restore(im);
        return ETH_HDR_LEN;

/* Get MAC address from card. */
    case NET_GET_HWADDR:
        restore(im);
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        addr->addr[0] = 0xAA;
        addr->addr[1] = 0xBB;
        addr->addr[2] = 0xCC;
        addr->addr[3] = 0xDD;
        addr->addr[4] = 0xEE;
        addr->addr[5] = 0xFF;
        break;

/* Get broadcast MAC address. */
    case NET_GET_HWBRC:
        restore(im);
        addr = (struct netaddr *)arg1;
        addr->type = NETADDR_ETHERNET;
        addr->len = ETH_ADDR_LEN;
        addr->addr[0] = 0xFF;
        addr->addr[1] = 0xFF;
        addr->addr[2] = 0xFF;
        addr->addr[3] = 0xFF;
        addr->addr[4] = 0xFF;
        addr->addr[5] = 0xFF;
        break;

/* Get MTU. */
    case NET_GET_MTU:
        restore(im);
        return ELOOP_MTU;

/* Get next packet off hold queue */
    case ELOOP_CTRL_GETHOLD:
        buf = (char *)arg1;
        /* Wait for held packet */
        wait(elpptr->hsem);
        /* Get and clear held packet */
        hold = elpptr->hold;
        holdlen = elpptr->holdlen;
        elpptr->hold = NULL;
        elpptr->holdlen = 0;
        restore(im);
        /* Copy held packet to buffer */
        if (arg2 < holdlen)
        {
            holdlen = arg2;
        }
        memcpy(buf, hold, holdlen);
        /* Free hold buffer */
        buffree(hold);
        return holdlen;

/* Set flags */
    case ELOOP_CTRL_SETFLAG:
        old = elpptr->flags & arg1;
        elpptr->flags |= (arg1);
        restore(im);
        return old;

/* Clear flags */
    case ELOOP_CTRL_CLRFLAG:
        old = elpptr->flags & arg1;
        elpptr->flags &= ~(arg1);
        restore(im);
        return old;

    default:
        restore(im);
        return SYSERR;
    }
    restore(im);
    return OK;
}
