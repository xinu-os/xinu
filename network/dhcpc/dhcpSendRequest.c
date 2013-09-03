/**
 * @file     dhcpSendRequest.c
 *
 */
/* Embedded Xinu, Copyright (C) 2008, 2013.  All rights reserved. */

#include <clock.h>
#include <device.h>
#include "dhcp.h"
#include <ethernet.h>
#include <ipv4.h>
#include <network.h>
#include <string.h>
#include <udp.h>

/**
 * Send a DHCPDISCOVER or DHCPREQUEST packet, depending on the current state of
 * the DHCP client (DHCPC_STATE_INIT or DHCPC_STATE_SELECTING, respectively).
 *
 * @param[in] descrp
 *      Network device on which to send the DHCP packet.
 * @param[in,out] data
 *      DHCP transfer data that has been filled in appropriately for
 *      DHCPC_STATE_INIT or DHCPC_STATE_SELECTING.
 *
 * @return
 *      OK if packet was successfully sent; SYSERR otherwise.
 */
syscall dhcpSendRequest(int descrp, struct dhcpData *data)
{
    struct packet *pkt;
    struct etherPkt *ether;
    struct ipv4Pkt *ipv4;
    struct udpPkt *udp;
    struct dhcpPkt *dhcp;
    int retval;
    uint pktsize;
    uchar optarray[128];
    uchar *opts;
    uint optsize;
    uint tmp_ipv4addr;

    /* Build DHCP options */
    opts = optarray;
    *opts++ = DHCP_OPT_MSGTYPE;
    *opts++ = 1;
    switch (data->state)
    {
        case DHCPC_STATE_INIT:
            *opts++ = DHCPDISCOVER;

            *opts++ = DHCP_OPT_PARAMREQ;
            *opts++ = 2;
            *opts++ = DHCP_OPT_SUBNET;
            *opts++ = DHCP_OPT_GATEWAY;
            break;

        case DHCPC_STATE_SELECTING:
            *opts++ = DHCPREQUEST;

            *opts++ = DHCP_OPT_REQUEST;
            *opts++ = IPv4_ADDR_LEN;
            tmp_ipv4addr = hl2net(data->offeredIpv4Addr);
            memcpy(opts, &tmp_ipv4addr, IPv4_ADDR_LEN);
            opts += IPv4_ADDR_LEN;

            *opts++ = DHCP_OPT_SERVER;
            *opts++ = IPv4_ADDR_LEN;
            tmp_ipv4addr = hl2net(data->serverIpv4Addr);
            memcpy(opts, &tmp_ipv4addr, IPv4_ADDR_LEN);
            opts += IPv4_ADDR_LEN;

            /* RFC 2131:  "If the client included a list of requested parameters
             * in a DHCPDISCOVER message, it MUST include that list in all
             * subsequent messages.  */
            *opts++ = DHCP_OPT_PARAMREQ;
            *opts++ = 2;
            *opts++ = DHCP_OPT_SUBNET;
            *opts++ = DHCP_OPT_GATEWAY;
            break;

        default:
            return SYSERR;
    }

    /* Add at least one DHCP_OPT_END byte to end the options, but add more as
     * needed so that the size of the options is a multiple of 4 bytes.  */
    do
    {
        *opts++ = DHCP_OPT_END;
    } while ((optsize = (opts - optarray)) & 3);

    /* Get memory for packet */
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        return SYSERR;
    }

    /* Set up pointers to other packet headers */
    pktsize = ETH_HDR_LEN + IPv4_HDR_LEN + UDP_HDR_LEN + DHCP_HDR_LEN + optsize;
    pkt->len += pktsize;
    pkt->curr -= pktsize;

    ether = (struct etherPkt*)pkt->curr;
    ipv4 = (struct ipv4Pkt *)ether->data;
    udp = (struct udpPkt *)ipv4->opts;
    dhcp = (struct dhcpPkt *)udp->data;

    /* Construct the DHCP packet */
    dhcp->op = DHCP_OP_REQUEST;
    dhcp->htype = DHCP_HTYPE_ETHER;
    dhcp->hlen = ETH_ADDR_LEN;
    dhcp->hops = 0;
    dhcp->xid = hl2net(data->cxid);
    if (data->state == DHCPC_STATE_SELECTING)
    {
        /* RFC 2131:  "... the DHCPREQUEST message MUST use the same value in
         * the DHCP message header's 'secs' field ... as the original
         * DHCPDISCOVER message."  */
        dhcp->secs = hs2net(data->discoverSecs);
    }
    else
    {
        dhcp->secs = hs2net(clktime - data->starttime);
        if (data->state == DHCPC_STATE_INIT)
        {
            data->discoverSecs = net2hs(dhcp->secs);
        }
    }

    dhcp->flags = 0x0000;
    dhcp->ciaddr = hl2net(data->clientIpv4Addr);
    dhcp->yiaddr = 0; /* RFC 2131:  client must set yiaddr to 0.  */
    dhcp->siaddr = 0; /* RFC 2131:  client must set siaddr to 0.  */
    dhcp->giaddr = 0; /* RFC 2131:  client must set giaddr to 0.  */
    memcpy(dhcp->chaddr, data->clientHwAddr, dhcp->hlen);
    dhcp->cookie = hl2net(DHCP_MAGICCOOKIE);
    memcpy(dhcp->opts, optarray, optsize);

    /* Construct UDP packet */
    udp->srcPort = hs2net(UDP_PORT_DHCPC);
    udp->dstPort = hs2net(UDP_PORT_DHCPS);
    udp->len = hs2net(UDP_HDR_LEN + DHCP_HDR_LEN + optsize);

    /* Setup part of IPv4 portion of packet for UDP psuedo header */
    ipv4->proto = IPv4_PROTO_UDP;
    memset(ipv4->src, 0, IPv4_ADDR_LEN);
    memset(ipv4->dst, 0xff, IPv4_ADDR_LEN);
    ipv4->ttl = 0;
    ipv4->chksum = udp->len;
    udp->chksum = netChksum((uchar *)&ipv4->ttl,
                            (sizeof(struct udpPseudoHdr) + UDP_HDR_LEN +
                             DHCP_HDR_LEN + optsize));

    /* Set up the IPv4 packet */
    ipv4->ver_ihl = (IPv4_VERSION << 4) | (IPv4_HDR_LEN / 4);
    ipv4->tos = IPv4_TOS_ROUTINE;
    ipv4->len = hs2net(pktsize - ETH_HDR_LEN);
    ipv4->id = 0;
    ipv4->flags_froff = hs2net(IPv4_FLAG_DF | 0x0000);
    ipv4->ttl = IPv4_TTL;
    ipv4->chksum = 0;
    ipv4->chksum = netChksum((uchar *)ipv4, IPv4_HDR_LEN);

    /* Set up the Ethernet packet and send it to the hardware broadcast address.
     * */
    memset(ether->dst, 0xff, ETH_ADDR_LEN);
    memcpy(ether->src, data->clientHwAddr, ETH_ADDR_LEN);
    ether->type = hs2net(ETHER_TYPE_IPv4);
    if (pktsize == write(descrp, pkt->curr, pktsize))
    {
        retval = OK;
    }
    else
    {
        retval = SYSERR;
    }
    netFreebuf(pkt);
    return retval;
}
