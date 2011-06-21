/**
 * @file etherRead.c
 * @provides etherRead.
 *
 * $Id: etherRead.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>
#include "bcm4713.h"
#include <string.h>
#include <interrupt.h>
#include <bufpool.h>
#include <stdlib.h>
#include <network.h>
#include <vlan.h>

/**
 * Read a packet from the ethernet device.
 * @param devptr pointer to ethernet device
 * @param buf buffer for read
 * @param len size of the buffer
 * @return number of bytes read
 */
devcall etherRead(device *devptr, void *buf, uint len)
{
    irqmask im;
    struct ether *ethptr;
    struct ethPktBuffer *pkt;
    struct rxHeader *rh;
    struct vlanPkt *lanptr;
    uint length;
    char *buffer = buf;

    ethptr = &ethertab[devptr->minor];

    im = disable();
    if (ETH_STATE_UP != ethptr->state)
    {
        restore(im);
        return SYSERR;
    }

    /* make sure buffer is large enough to store packet */
    if (len < ETH_HEADER_LEN)
    {
        restore(im);
        return SYSERR;
    }

    wait(ethptr->isema);

    pkt = ethptr->in[ethptr->istart];
    ethptr->in[ethptr->istart] = NULL;
    ethptr->istart = (ethptr->istart + 1) % ETH_IBLEN;
    ethptr->icount--;
    restore(im);

    if (NULL == pkt)
    {
        return 0;
    }
    rh = (struct rxHeader *)pkt->buf;
    lanptr = (struct vlanPkt *)pkt->data;

    /* slice off CRC */
    rh->length -= ETH_CRC_LEN;

    if (ETH_TYPE_VLAN == net2hs(lanptr->tpi))
    {
        rh->length -= 4;        /* Account for vlan tag removal */
    }

    /* compute size of packet */
    length = (rh->length < len) ? rh->length : len;

    if (ETH_TYPE_VLAN == net2hs(lanptr->tpi))
    {
        /* strip vlan tag */
        memcpy(buffer, pkt->data, 12);
        memcpy(buffer + 12, pkt->data + 16, length - 12);
    }
    else
    {
        /* no vlan tagging */
        memcpy(buffer, pkt->data, length);
    }

    buffree(pkt);

    return length;
}
