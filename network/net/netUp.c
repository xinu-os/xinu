/**
 * @file netUp.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <conf.h>
#include <device.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

static int netAlloc(void);

/**
 * @ingroup network
 *
 * Starts a network interface using the specified protocol addresses.
 *
 * @param descrp
 *      Index of the underlying network device on which to open the interface.
 * @param ip
 *      Protocol address; cannot be NULL.
 * @param mask
 *      Protocol address mask; cannot be NULL.
 * @param gateway
 *      Protocol address of the gateway, or NULL if unspecified.  If
 *      unspecified, it is interpreted as "no gateway".
 *
 * @return
 *      OK if the network interface was successfully started; otherwise SYSERR.
 */
syscall netUp(int descrp, const struct netaddr *ip, const struct netaddr *mask,
              const struct netaddr *gateway)
{
    irqmask im;
    int nif;
    struct netif *netptr;
    uint i;
    uint nthreads;
    int retval = SYSERR;

    /* Error check arguments */
    if (isbaddev(descrp) || NULL == ip || NULL == mask)
    {
        NET_TRACE("Bad device, null IP, or null mask");
        goto out;
    }

    if (ip->len > NET_MAX_ALEN || mask->len > NET_MAX_ALEN ||
        (NULL != gateway && gateway->len > NET_MAX_ALEN))
    {
        NET_TRACE("IP, mask, and/or gateway are longer than maximum "
                  "network address length.");
        goto out;
    }
    im = disable();

    /* Ensure network interface is not already started on underlying device */
    netptr = netLookup(descrp);
    if (NULL != netptr)
    {
        NET_TRACE("Network interface is already started on underlying device.");
        goto out_restore;
    }

    /* Select a free network interface */
    nif = netAlloc();
    if (SYSERR == nif)
    {
        NET_TRACE("Failed to allocate a network interface.");
        goto out_restore;
    }
    netptr = &netiftab[nif];
    NET_TRACE("Starting netif %d on device %d", nif, descrp);

    /* Initialize the network interface structure */
    bzero(netptr, sizeof(struct netif));
    netptr->dev = descrp;
    netptr->state = NET_ALLOC;
    netptr->mtu = control(descrp, NET_GET_MTU, 0, 0);
    netptr->linkhdrlen = control(descrp, NET_GET_LINKHDRLEN, 0, 0);
    if (SYSERR == netptr->mtu || SYSERR == netptr->linkhdrlen)
    {
        NET_TRACE("Failed to get MTU and/or link header length\n");
        goto out_free_nif;
    }

    /* Get NIC hardware address and hardware broadcast address  */
    if ((SYSERR ==
         control(descrp, NET_GET_HWADDR, (long)&netptr->hwaddr, 0))
        || (SYSERR ==
            control(descrp, NET_GET_HWBRC, (long)&netptr->hwbrc, 0)))
    {
        NET_TRACE("Couldn't get NIC addresses");
        goto out_free_nif;
    }

    /* Set protocol addresses */
    netaddrcpy(&netptr->ip, ip);
    netaddrcpy(&netptr->mask, mask);
    if (NULL != gateway)
    {
        netaddrcpy(&netptr->gateway, gateway);
    }
    /* The following loop will turn, for example, an IPv4 mask of 255.255.255.0
     * and IPv4 address of 192.168.0.50 into an IPv4 broadcast address of
     * 192.168.0.255.  */
    netptr->ipbrc.type = netptr->ip.type;
    netptr->ipbrc.len = netptr->ip.len;
    for (i = 0; i < netptr->ip.len; i++)
    {
        netptr->ipbrc.addr[i] = netptr->ip.addr[i] & netptr->mask.addr[i];
        netptr->ipbrc.addr[i] |= ~netptr->mask.addr[i];
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

    /* TODO: Get hostname from nvram (if nvram available)  */

    /* Add subnet address and gateway to the route table */
    rtAdd(&netptr->ip, NULL, &netptr->mask, netptr);
    if (NULL != gateway)
    {
        rtDefault(&netptr->gateway, netptr);
    }

    /*  Spawn receive threads associated with this interface */
    for (i = 0; i < NET_NTHR; i++)
    {
        char thrname[DEVMAXNAME + 30];
        tid_typ tid;

        sprintf(thrname, "%srecv%02d", devtab[descrp].name, i);
        tid = create(netRecv, NET_THR_STK, NET_THR_PRIO, thrname, 1, netptr);
        if (SYSERR == tid)
        {
            /* Failed to create all receive threads; kill the ones that have
             * already been spawned.  */
            nthreads = i;
            goto out_kill_recv_threads;
        }
        netptr->recvthr[i] = tid;
        ready(tid, RESCHED_NO);
    }

    retval = OK;
    goto out_restore;

out_kill_recv_threads:
    for (i = 0; i < nthreads; i++)
    {
        kill(netptr->recvthr[i]);
    }
out_free_nif:
    netptr->state = NET_FREE;
out_restore:
    restore(im);
out:
    return retval;
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
