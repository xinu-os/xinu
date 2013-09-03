/**
 * @file dhcpc.h
 *
 * DHCP client interface
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _DHCPC_H_
#define _DHCPC_H_

#include <stddef.h>
#include <network.h>

/** DHCP transfer data */
struct dhcpData
{
    /* DHCP client output */
    struct netaddr ip;
    struct netaddr gateway;
    struct netaddr mask;
    char bootfile[128];
    struct netaddr next_server;

    /* DHCP client internal variables */
    int state;
    uint cxid;
    uint starttime;
    int recvStatus;
    ushort discoverSecs;
    uint offeredIpv4Addr;
    uint clientIpv4Addr;
    uint serverIpv4Addr;
    uchar clientHwAddr[ETH_ADDR_LEN];
    uchar serverHwAddr[ETH_ADDR_LEN];
};

syscall dhcpClient(int descrp, uint timeout, struct dhcpData *data);

#endif /* _DHCPC_H_ */
