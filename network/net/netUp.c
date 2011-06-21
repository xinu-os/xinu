/**
 * @file netUp.c
 * @provides netUp
 *
 * $Id: netUp.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

static int netAlloc(void);

/**
 * Start a network interface 
 * @param descrp underlying device
 * @param ip Protocol address
 * @param mask Protocol address mask
 * @param gateway Protocol address of the gateway
 * @return OK if started properly, otherwise SYSERR
 */
syscall netUp(int descrp, struct netaddr *ip, struct netaddr *mask,
              struct netaddr *gateway)
{
    irqmask im;
    struct netif *netptr;
    int nif = 0;
    int i = 0;
    char thrname[TNMLEN];
    tid_typ tid;
    device *devptr;

    /* Error check arguments */
    if (isbaddev(descrp) || NULL == ip || NULL == mask)
    {
        NET_TRACE("Bad device, null IP, or null mask");
        return SYSERR;
    }
    if ((ip->len > NET_MAX_ALEN) || (mask->len > NET_MAX_ALEN))
    {
        NET_TRACE
            ("IP or mask are longer than maximum network address length.");
        return SYSERR;
    }

    im = disable();

    /* Ensure network interface is not already started on underlying device */
#if NNETIF
    for (i = 0; i < NNETIF; i++)
    {
        if ((NET_ALLOC == netiftab[i].state)
            && (descrp == netiftab[i].dev))
        {
            NET_TRACE
                ("Network interface is already started on underlying device.");
            return SYSERR;
        }
    }
#endif
    /* Select a free network interface */
    nif = netAlloc();
    if (SYSERR == nif)
    {
        NET_TRACE("Failed to allocate a network interface.");
        restore(im);
        return SYSERR;
    }
    netptr = &netiftab[nif];
    NET_TRACE("Starting netif %d on device %d", nif, descrp);

    /* Store configuration information */
    netptr->state = NET_ALLOC;
    netptr->dev = descrp;
    netaddrcpy(&netptr->ip, ip);
    netaddrcpy(&netptr->mask, mask);
    if (NULL == gateway)
    {
        netptr->gateway.type = NULL;
    }
    else
    {
        netaddrcpy(&netptr->gateway, gateway);
    }
    netptr->ipbrc.type = netptr->ip.type;
    netptr->ipbrc.len = netptr->ip.len;
    for (i = 0; (i < netptr->ip.len) && (i < NET_MAX_ALEN); i++)
    {
        netptr->ipbrc.addr[i] = netptr->ip.addr[i] & netptr->mask.addr[i];
        netptr->ipbrc.addr[i] |= ~netptr->mask.addr[i];
    }
    netptr->linkhdrlen = control(descrp, NET_GET_LINKHDRLEN, NULL, NULL);
    netptr->mtu = control(descrp, NET_GET_MTU, NULL, NULL);
    if ((SYSERR == (int)netptr->linkhdrlen)
        || (SYSERR == (int)netptr->mtu))
    {
        netptr->state = NET_FREE;
        restore(im);
        NET_TRACE("Hdrlen = %d, MTU = %d",
                  (int)netptr->linkhdrlen, (int)netptr->mtu);
        return SYSERR;
    }
    if ((SYSERR ==
         control(descrp, NET_GET_HWADDR, (int)&netptr->hwaddr, NULL))
        || (SYSERR ==
            control(descrp, NET_GET_HWBRC, (int)&netptr->hwbrc, NULL)))
    {
        netptr->state = NET_FREE;
        restore(im);
        NET_TRACE("Couldn't get NIC addresses");
        return SYSERR;
    }

#ifdef TRACE_NET
    char str[20];
    netaddrsprintf(str, &netptr->ip);
    NET_TRACE("\tIP %s", str);
    netaddrsprintf(str, &netptr->mask);
    NET_TRACE("\tMask %s", str);
    netaddrsprintf(str, &netptr->gateway);
    NET_TRACE("\tGateway %s", str);
    netaddrsprintf(str, &netptr->hwaddr);
    NET_TRACE("\tHw Addr %s", str);
    netaddrsprintf(str, &netptr->hwbrc);
    NET_TRACE("\tHw Broadcast %s", str);
    NET_TRACE("\tMTU %d\tLink header len %d",
              netptr->mtu, netptr->linkhdrlen);
#endif


    /* TODO: Get hostname from nvram */

    /* Add subnet address and gateway to the route table */
    rtAdd(&netptr->ip, NULL, &netptr->mask, netptr);
    if (gateway != NULL)
    {
        rtDefault(&netptr->gateway, netptr);
    }

    devptr = (device *)&devtab[netptr->dev];
    /*  Spawn receive threads associated with this interface */
    for (i = 0; i < NET_NTHR; i++)
    {
        sprintf(thrname, "%srecv%02d", devptr->name, i);
        tid = create((void *)netRecv, NET_THR_STK, NET_THR_PRIO,
                     thrname, 1, netptr);
        netptr->recvthr[i] = tid;
        ready(tid, RESCHED_NO);
    }
    restore(im);

    return OK;
}

/**
 * Obtain a free network interface.
 * @return a free network interface id, SYSERR if all netifs are used 
 */
static int netAlloc(void)
{
#if NNETIF
    int nif = 0;

    /* Check all NNETIF slots */
    for (nif = 0; nif < NNETIF; nif++)
    {
        if (NET_FREE == netiftab[nif].state)
        {
            return nif;
        }
    }
#endif
    return SYSERR;
}
