/**
 * @file     udpSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <ipv4.h>
#include <network.h>
#include <string.h>
#include <udp.h>

/**
 * @ingroup udpinternal
 *
 * Send a UDP packet through a UDP device, thereby sending it over the network
 * with the address/port parameters with which the UDP device is configured.
 *
 * This function is intended to be internal to the UDP code.  From external code
 * use write(), which calls udpWrite().
 *
 * Note: depending on the lower levels of the network stack, this function
 * likely only buffers the packet to be sent at some later time.  Therefore, it
 * may not have actually been transmitted on the wire when this function
 * returns.
 *
 * The UDP device MUST be open and MUST remain open throughout the execution of
 * this function.
 *
 * @param udpptr
 *      Pointer to the control block for the UDP device.
 * @param datalen
 *      Number of bytes of data to send, which must be a valid value for the
 *      current UDP device mode (e.g. if in passive mode, it must be at least
 *      the length of the UDP pseudo-header plus UDP header).
 * @param buf
 *      Buffer of data to send.  In the default mode, this will interpreted as
 *      the UDP payload; however, if the UDP device is in passive mode, this
 *      will interpreted as the UDP pseudo-header follewed by the UDP header
 *      followed by the UDP payload.
 *
 * @return OK if packet was sent successfully; otherwise SYSERR or an error code
 *         returned by ipv4Send().
 */
syscall udpSend(struct udp *udpptr, ushort datalen, const void *buf)
{
    struct packet *pkt;
    struct udpPkt *udppkt;
    struct netaddr localip, remoteip;
    int result;

    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        UDP_TRACE("Failed to allocate buffer");
        return SYSERR;
    }

    netaddrcpy(&localip, &(udpptr->localip));
    netaddrcpy(&remoteip, &(udpptr->remoteip));

    if (udpptr->flags & UDP_FLAG_PASSIVE)
    {
        const struct udpPseudoHdr *pseudo;

        pseudo = buf;
        datalen -= sizeof(struct udpPseudoHdr);
        /* Set the length of the packet and set the curr pointer back that
         * length */
        pkt->len = datalen;
        /* Round the datalength to maintain word alignment */
        pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

        /* Copy packet plus header */
        memcpy(pkt->curr, (pseudo + 1), datalen);
        udppkt = (struct udpPkt *)(pkt->curr);
        /* Prep fields for network order */
        udppkt->srcPort = hs2net(udppkt->srcPort);
        udppkt->dstPort = hs2net(udppkt->dstPort);
        udppkt->len = hs2net(pkt->len);
        udppkt->chksum = 0;

        remoteip.len = localip.len;
        remoteip.type = localip.type;
        memcpy(&remoteip.addr, pseudo->dstIp, remoteip.len);

        if (0 != memcmp(&localip.addr, pseudo->srcIp, localip.len))
        {
            UDP_TRACE("Src IP does not match UDP passive IP.");
            netFreebuf(pkt);
            return SYSERR;
        }
    }
    else
    {
        datalen += UDP_HDR_LEN;
        /* Set the length of the packet and set the curr pointer back that
         * length */
        pkt->len = datalen;
        /* Round the datalength to maintain word alignment */
        pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

        /* Set UDP header fields and fill the packet with the data */
        udppkt = (struct udpPkt *)(pkt->curr);
        udppkt->srcPort = hs2net(udpptr->localpt);
        udppkt->dstPort = hs2net(udpptr->remotept);
        udppkt->len = hs2net(pkt->len);
        udppkt->chksum = 0;

        memcpy(udppkt->data, buf, datalen - UDP_HDR_LEN);
    }

    /* Calculate UDP checksum (which happens to be the same as TCP's) */
    udppkt->chksum = udpChksum(pkt, datalen, &localip, &remoteip);

    /* Send the UDP packet through IP */
    result = ipv4Send(pkt, &localip, &remoteip, IPv4_PROTO_UDP);

    if (SYSERR == netFreebuf(pkt))
    {
        UDP_TRACE("Failed to free buffer");
        return SYSERR;
    }

    return result;
}
