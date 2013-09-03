/**
 * file ipv4SendFrag.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>
#include <icmp.h>
#include <network.h>
#include <stdlib.h>
#include <string.h>
#include <ethernet.h>

/**
 * Fragments packet into maximum transmission unit sized chunks.
 * @param pkt the packet to fragment
 * @return OK
 */
syscall ipv4SendFrag(struct packet *pkt, struct netaddr *nxthop)
{
    uint ihl;
    uchar *data;
    uint dRem = 0;              // The amount of data remaining to be fragmented
    ushort froff;
    ushort lastFlag;
    ushort dLen;

    // Incoming packet structures
    struct ipv4Pkt *ip;

    // Outgoing packet structures
    struct ipv4Pkt *outip;
    struct packet *outpkt;

    IPv4_TRACE("Declaration");

    if (NULL == pkt)
    {
        return SYSERR;
    }

    // Setup incoming packet structures
    ip = (struct ipv4Pkt *)pkt->curr;

    if (net2hs(ip->len) <= pkt->nif->mtu)
    {
        IPv4_TRACE("NetSend");

        if (netaddrequal(&pkt->nif->ipbrc, nxthop))
        {
            IPv4_TRACE("Subnet Broadcast");
            return netSend(pkt, &NETADDR_GLOBAL_ETH_BRC,
                           nxthop, ETHER_TYPE_IPv4);
        }
        return netSend(pkt, NULL, nxthop, ETHER_TYPE_IPv4);
    }

    // Verify header does not have DF
    if (net2hs(ip->flags_froff) & IPv4_FLAG_DF)
    {
        IPv4_TRACE("net2hs of froff");
        /* Send ICMP message */
        icmpDestUnreach(pkt, ICMP_FOFF_DFSET);
        return SYSERR;
    }

    ihl = (ip->ver_ihl & IPv4_IHL) * 4;
    data = ((uchar *)ip) + ihl;
    dRem = net2hs(ip->len) - ihl;
    froff = net2hs(ip->flags_froff) & IPv4_FROFF;
    lastFlag = net2hs(ip->flags_froff) & IPv4_FLAGS;

    // Length of data in this packet will be MTU - header length,
    //  rounded down to nearest multiple of 8 bytes.
    dLen = (pkt->nif->mtu - ihl) & ~0x7;

    pkt->len = ihl + dLen;
    ip->len = hs2net(pkt->len);

    // Set more fragments flag
    ip->flags_froff = IPv4_FLAG_MF & froff;
    ip->flags_froff = hs2net(ip->flags_froff);

    ip->chksum = 0;
    ip->chksum = netChksum((uchar *)ip, ihl);

    netSend(pkt, NULL, nxthop, ETHER_TYPE_IPv4);
    dRem -= dLen;
    data += dLen;
    froff += (dLen / 8);

    // Get memory from stack for outgoing fragment
    outpkt = netGetbuf();

    if (SYSERR == (int)outpkt)
    {
        IPv4_TRACE("allocating outpkt");
        return SYSERR;
    }

    // Set up outgoing packet pointers and variables
    outpkt->curr -= pkt->nif->mtu;
    outip = (struct ipv4Pkt *)outpkt->curr;
    outpkt->nif = pkt->nif;

    memcpy(outip, ip, IPv4_HDR_LEN);

    // While packet must be fragmented
    while (dRem > 0)
    {
        if (((dRem + 7) & ~0x7) > pkt->nif->mtu + IPv4_HDR_LEN)
        {
            dLen = (pkt->nif->mtu - IPv4_HDR_LEN) & ~0x7;
        }
        else
        {
            dLen = dRem;
        }
        // Copy data segment
        memcpy(outip->opts, data, dLen);

        // Set more fragments flag
        if (dLen == dRem)
        {
            outip->flags_froff = lastFlag & froff;
        }
        else
        {
            outip->flags_froff = IPv4_FLAG_MF & froff;
        }
        outip->flags_froff = hs2net(outip->flags_froff);

        // Update fields
        outip->len = hs2net(IPv4_HDR_LEN + dLen);
        outip->chksum = 0;
        outip->chksum = netChksum((uchar *)outip, IPv4_HDR_LEN);

        // Update outgoing packet length
        outpkt->len = net2hs(outip->len);

        // Send fragment
        netSend(outpkt, NULL, nxthop, ETHER_TYPE_IPv4);

        dRem -= dLen;
        data += dLen;
        froff += (dLen / 8);
    }

    IPv4_TRACE("freeing outpkt");
    netFreebuf(outpkt);
    return OK;
}
