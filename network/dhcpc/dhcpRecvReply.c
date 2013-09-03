/**
 * @file     dhcpRecvReply.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008, 2013.  All rights reserved. */

#include <device.h>
#include "dhcp.h"
#include <ether.h>
#include <ipv4.h>
#include <network.h>
#include <string.h>
#include <stdlib.h>
#include <udp.h>
#include <thread.h>

#define DHCP_RECV_STKSIZE NET_THR_STK
#define DHCP_RECV_PRIO    NET_THR_PRIO

/* Stress testing--- randomly ignore this percent of valid received data
 * packets.  */
#define DHCP_DROP_PACKET_PERCENT 0

static thread do_dhcpRecvReply(int descrp, struct dhcpData *data,
                               struct packet *pkt);

/**
 * Wait, with timeout, for a response from a DHCP server and update the DHCP
 * transfer data.  The exact behavior of this function depends on the current
 * DHCP client state:
 *
 * - If in DHCPC_STATE_SELECTING, the client waits until it gets a DHCPOFFER
 *   reply from any server.
 * - If in DHCPC_STATE_REQUESTING, the client waits until it gets a DHCPACK or
 *   DHCPNAK reply from the server to which it sent the DHCPREQUEST.
 *
 * @param[in] descrp
 *      Network device on which to wait for a response.
 * @param[in,out] data
 *      DHCP transfer data.
 * @param[in] timeout
 *      Milliseconds to wait before timing out.
 *
 * @return
 *      OK if successful; TIMEOUT if timed out; SYSERR if other error occurred.
 */
syscall dhcpRecvReply(int descrp, struct dhcpData *data, uint timeout)
{
    struct packet *pkt;
    tid_typ tid;
    int retval;

    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        return SYSERR;
    }

    /* This somewhat of a hack to implement a timeout:  Wait for the reply in
     * another thread to avoid blocking on read().  */
    tid = create(do_dhcpRecvReply, DHCP_RECV_STKSIZE, DHCP_RECV_PRIO,
                 "dhcpRecvReply", 3, descrp, data, pkt);
    if (isbadtid(tid))
    {
        netFreebuf(pkt);
        return SYSERR;
    }

    ready(tid, RESCHED_YES);

    /* Wait at most @timeout milliseconds for the thread to terminate before
     * returning TIMEOUT.  */
    if (TIMEOUT == recvtime(timeout))
    {
        kill(tid);
        receive();
        retval = TIMEOUT;
    }
    else
    {
        retval = data->recvStatus;
    }
    netFreebuf(pkt);
    return retval;
}

/**
 * The comments for dhcpRecvReply() apply, but for do_dhcpRecvReply() there is
 * no timeout and it is executed in a separate thread; furthermore, since this
 * thread can be killed at any time, it must use the memory passed in the @p pkt
 * parameter rather than allocating its own memory.
 */
static thread do_dhcpRecvReply(int descrp, struct dhcpData *data,
                               struct packet *pkt)
{
    const struct etherPkt *epkt;
    const struct ipv4Pkt *ipv4;
    const struct udpPkt *udp;
    const struct dhcpPkt *dhcp;
    const uchar *opts;
    uint maxlen;
    int found_msg;
    int retval;
    const uchar *gatewayptr;
    const uchar *maskptr;
    const uchar *opts_end;
    uint serverIpv4Addr;
    int mtu;
    int linkhdrlen;

    mtu = control(descrp, NET_GET_MTU, 0, 0);
    linkhdrlen = control(descrp, NET_GET_LINKHDRLEN, 0, 0);
    if (SYSERR == mtu || SYSERR == linkhdrlen)
    {
        return SYSERR;
    }

    maxlen = linkhdrlen + mtu;

    /* Receive packets until we find a response we're waiting for.  */

next_packet:
    do
    {
        /* Receive next packet from the network device.  */
        int len = read(descrp, pkt->data, maxlen);
        if (len == SYSERR || len <= 0)
        {
            data->recvStatus = SYSERR;
            return SYSERR;
        }

        pkt->len = len;
        DHCP_TRACE("Received packet (len=%u).", pkt->len);

        /* Make sure packet is at least the minimum length of a DHCP packet.  */
        if (pkt->len < (ETH_HDR_LEN + IPv4_HDR_LEN +
                        UDP_HDR_LEN + DHCP_HDR_LEN))
        {
            DHCP_TRACE("Too short to be a DHCP packet.");
            goto next_packet;
        }

        /* Set up header pointers  */
        epkt = (const struct etherPkt *)pkt->data;
        ipv4 = (const struct ipv4Pkt *)epkt->data;
        udp = (const struct udpPkt *)ipv4->opts;
        dhcp = (const struct dhcpPkt *)udp->data;

        /* DHCP packets must be type IPv4, protocol UDP, UDP dest port DHCPC,
         * and UDP source port DHCPS.
         *
         * Also check the DHCP header:  Is it actually a reply to this client?
         * */
        if ((ETHER_TYPE_IPv4 != net2hs(epkt->type))
            || (IPv4_PROTO_UDP != ipv4->proto)
            || (UDP_PORT_DHCPC != net2hs(udp->dstPort))
            || (UDP_PORT_DHCPS != net2hs(udp->srcPort))
            || (DHCP_OP_REPLY != dhcp->op)
            || (data->cxid != net2hl(dhcp->xid)))
        {
            DHCP_TRACE("Not a DHCP reply to this client.");
            goto next_packet;
        }

        DHCP_TRACE("Received DHCP reply.");

    #if DHCP_DROP_PACKET_PERCENT != 0
        /* Stress testing.  */
        if (rand() % 100 < DHCP_DROP_PACKET_PERCENT)
        {
            DHCP_TRACE("WARNING: Ignoring valid DHCP packet for test purposes.");
            goto next_packet;
        }
    #endif

        /* We got a valid DHCP reply.  Now parse the DHCP options.  This needs
         * to be done carefully to avoid overrunning the packet buffer if the
         * options data is invalid.  */
        opts = dhcp->opts;
        maskptr = NULL;
        gatewayptr = NULL;
        serverIpv4Addr = 0;
        opts_end = opts + (pkt->len - (ETH_HDR_LEN + IPv4_HDR_LEN +
                                       UDP_HDR_LEN + DHCP_HDR_LEN));
        found_msg = -1;
        for (;;)
        {
            uchar opt, len;

            /* Get the next option type.  */
            if (opts >= opts_end)
            {
                DHCP_TRACE("Invalid DHCP options.");
                goto next_packet;
            }
            opt = *opts++;

            /* Break on DHCP_OPT_END (marks end of DHCP options).  */
            if (DHCP_OPT_END == opt)
            {
                DHCP_TRACE("Reached DHCP_OPT_END.");
                break;
            }

            /* Get length of the data for this option.  */
            if (opts >= opts_end)
            {
                DHCP_TRACE("Invalid DHCP options.");
                goto next_packet;
            }
            len = *opts++;

            if (opts + len >= opts_end)
            {
                DHCP_TRACE("Invalid DHCP options.");
                goto next_packet;
            }

            /* Process the specific DHCP option.  Ignore unrecognized options.
             * */
            switch (opt)
            {
            case DHCP_OPT_MSGTYPE:
                DHCP_TRACE("DHCP_OPT_MSGTYPE: %d", *opts);
                if (len >= 1)
                {
                    if ((DHCPC_STATE_SELECTING == data->state &&
                         *opts == DHCPOFFER)
                        || (DHCPC_STATE_REQUESTING == data->state &&
                            (DHCPACK == *opts || DHCPNAK == *opts)))
                    {
                        found_msg = *opts;
                    }
                }
                break;

            case DHCP_OPT_SUBNET:
                if (len >= IPv4_ADDR_LEN)
                {
                    maskptr = opts;
                }
                break;

            case DHCP_OPT_GATEWAY:
                if (len >= IPv4_ADDR_LEN)
                {
                    gatewayptr = opts;
                }
                break;

            case DHCP_OPT_SERVER:
                if (len >= IPv4_ADDR_LEN)
                {
                    /* Server Identifier option.  */
                    serverIpv4Addr = ((uint)opts[0] << 24) |
                                     ((uint)opts[1] << 16) |
                                     ((uint)opts[2] << 8) |
                                     ((uint)opts[3] << 0);
                }
                break;
            }

            /* Advance by the length of the option's data.  */
            opts += len;
        }
    } while (found_msg < 0);

    /* We received a reply of at least the right type as one we were looking
     * for.  */

    /* Don't consider a DHCPACK reply to be valid unless it provided a subnet
     * mask.  */
    if (DHCPACK == found_msg && NULL == maskptr)
    {
        DHCP_TRACE("Ignoring DHCPACK (no subnet mask provided).");
        goto next_packet;
    }

    /* Note: The server's IP address is supposed to be specified in the Server
     * Identifier option, *not* in the siaddr (Server IP Address) field.  This
     * is because, somewhat unintuitively, siaddr is used for the address of the
     * next server in the bootstrap process, which may not be the same as the
     * DHCP server.  But if the Server Identifier option wasn't present, use
     * siaddr anyway.  */
    if (0 == serverIpv4Addr)
    {
        serverIpv4Addr = net2hl(dhcp->siaddr);
        if (0 == serverIpv4Addr)
        {
            DHCP_TRACE("Server IP address empty.");
            goto next_packet;
        }
    }

    if (DHCPOFFER == found_msg)
    {
        /* DHCPOFFER:  Remember offer and server addresses.  */
        data->serverIpv4Addr = serverIpv4Addr;
        data->offeredIpv4Addr = net2hl(dhcp->yiaddr);
        memcpy(data->serverHwAddr, epkt->src, ETH_ADDR_LEN);
        retval = OK;
    }
    else
    {
        /* Received DHCPACK or DHCPNAK.  Ensure it's from the same server to
         * which we sent the DHCPREQUEST; if not, ignore the packet.  */
        if (serverIpv4Addr != data->serverIpv4Addr)
        {
            DHCP_TRACE("Reply from wrong server.");
            goto next_packet;
        }

        if (DHCPNAK == found_msg)
        {
            /* DHCPNAK:  Return error to make client try DHCPDISCOVER again  */
            retval = SYSERR;
        }
        else
        {
            /* DHCPACK:  Set network interface addresses  */
            data->ip.type = NETADDR_IPv4;
            data->ip.len = IPv4_ADDR_LEN;
            /* yiaddr in a DHCPACK should be the same as the yiaddr stored from
             * the DHCPOFFER, but using the value in DHCPACK is preferable since
             * it's the value the server thinks it assigned.  */
            memcpy(data->ip.addr, &dhcp->yiaddr, IPv4_ADDR_LEN);
            data->clientIpv4Addr = net2hl(dhcp->yiaddr);

            data->mask.type = NETADDR_IPv4;
            data->mask.len = IPv4_ADDR_LEN;
            memcpy(data->mask.addr, maskptr, IPv4_ADDR_LEN);

            if (NULL != gatewayptr)
            {
                data->gateway.type = NETADDR_IPv4;
                data->gateway.len = IPv4_ADDR_LEN;
                memcpy(data->gateway.addr, gatewayptr, IPv4_ADDR_LEN);
            }

            /* If provided in the DHCPACK, set the address of next server and
             * the boot file (e.g. for TFTP).  */
            if (0 != dhcp->siaddr)
            {
                data->next_server.type = NETADDR_IPv4;
                data->next_server.len = IPv4_ADDR_LEN;
                memcpy(data->next_server.addr, &dhcp->siaddr, IPv4_ADDR_LEN);
            }
            if ('\0' != dhcp->file[0])
            {
                memcpy(data->bootfile, dhcp->file, sizeof(data->bootfile) - 1);
            }
        #ifdef ENABLE_DHCP_TRACE
            {
                char str_addr[24];
                netaddrsprintf(str_addr, &data->ip);
                DHCP_TRACE("Set ip=%s", str_addr);

                netaddrsprintf(str_addr, &data->mask);
                DHCP_TRACE("Set mask=%s", str_addr);

                if (NULL != gatewayptr)
                {
                    netaddrsprintf(str_addr, &data->gateway);
                    DHCP_TRACE("Set gateway=%s", str_addr);
                }
                else
                {
                    DHCP_TRACE("No gateway.");
                }

                netaddrsprintf(str_addr, &data->next_server);
                DHCP_TRACE("TFTP server=%s", str_addr);
                DHCP_TRACE("Bootfile=%s", data->bootfile);
            }
        #endif
            retval = OK;
        }
    }
    data->recvStatus = retval;
    return retval;
}
