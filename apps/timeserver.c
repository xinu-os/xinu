/*
 * @file     timeserver.c
 *
 */
/* Embedded Xinu, Copyright (C) 2010.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <device.h>
#include <udp.h>
#include <date.h>
#include <stdlib.h>
#include <shell.h>
#include <ether.h>

/**
 * Time server daemon provides RFC 868 Time Protocol (UDP) service.
 * @param descrp network interface to listen on
 * @param localport UDP port to listen on
 * @return non-zero value on error
 */
thread timeServer(int descrp, int localport)
{
    ushort dev = 0, remoteport = 0;
    int len = 0;
    long time = 0;
    char buffer[TIMESERVER_BUFSIZE];
    struct netif *interface = NULL;
    struct netaddr *localhost = NULL;
    struct netaddr remotehost;
    struct udpPseudoHdr *pseudo = NULL;
    struct udpPkt *udp = NULL;

    dev = udpAlloc();
    /* Open Device on rdate port */
    if ((ushort)SYSERR == dev)
    {
        fprintf(stderr, "No UDP devices available\n");
        return SYSERR;
    }

    /* Look up local ip info */
    interface = netLookup(descrp);

    if (NULL == interface)
    {
        fprintf(stderr, "No network interface found\n");
        return 1;
    }

    localhost = &(interface->ip);

    if (SYSERR == open(dev, localhost, NULL, localport, NULL))
    {
        fprintf(stderr, "Error opening UDP device\n");
        return SYSERR;
    }

    if (SYSERR == control(dev, UDP_CTRL_SETFLAG, UDP_FLAG_PASSIVE, NULL))
    {
        fprintf(stderr, "Error setting UDP device to PASSIVE\n");
        close(dev);
        return SYSERR;
    }

    netaddrcpy(&remotehost, localhost);

    while (SYSERR != (len = read(dev, buffer, TIMESERVER_BUFSIZE)))
    {
        pseudo = (struct udpPseudoHdr *)buffer;
        udp = (struct udpPkt *)(pseudo + 1);

        memcpy(remotehost.addr, pseudo->srcIp, remotehost.len);
        remoteport = udp->srcPort;

        memcpy(pseudo->srcIp, pseudo->dstIp, remotehost.len);
        memcpy(pseudo->dstIp, remotehost.addr, remotehost.len);
        pseudo->len += 4;
        udp->srcPort = udp->dstPort;
        udp->dstPort = remoteport;
        udp->len += 4;

        time = hl2net(get_datetime());
        memcpy(udp->data, &time, sizeof(time));
        if (SYSERR == write(dev, buffer, len + 4))
        {
            fprintf(stderr, "Error writing rdate reply\n");
            close(dev);
            return SYSERR;
        }

        /* printf("rdate server: %d.%d.%d.%d",  */
        /*         pseudo->srcIp[0], */
        /*         pseudo->srcIp[1], */
        /*         pseudo->srcIp[2], */
        /*         pseudo->srcIp[3]); */
        /* printf(":%d -> ", udp->srcPort); */
        /* printf("%d.%d.%d.%d",  */
        /*         pseudo->dstIp[0], */
        /*         pseudo->dstIp[1], */
        /*         pseudo->dstIp[2], */
        /*         pseudo->dstIp[3]); */
        /* printf(":%d 0x%08X %d\n", udp->dstPort, time, time); */
    }
    close(dev);
    return OK;
}
