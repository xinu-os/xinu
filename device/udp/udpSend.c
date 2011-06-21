/**
 * @file     udpSend.c
 * @provides udpSend
 *
 * $Id: udpSend.c 2079 2009-10-05 22:32:48Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <device.h>
#include <ipv4.h>
#include <kernel.h>
#include <network.h>
#include <udp.h>

/**
 * Send a UDP packet across the network to a certain destination
 * @param udpptr Pointer to UDP control block
 * @param datalen Length of data to be sent
 * @param buf Data to be sent
 * @return OK if packet is sent properly, otherwise SYSERR
 */
syscall udpSend(struct udp *udpptr, ushort datalen, void *buf)
{
    struct packet *pkt;
    struct udpPkt *udppkt = NULL;
    int result;

    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        return SYSERR;
    }

    /* Set the length of the packet and set the curr pointer back that 
     * length */
    pkt->len = UDP_HDR_LEN + datalen;
    /* Round the datalength to maintain word alignment */
    pkt->curr -= (3 + (ulong)(pkt->len)) & ~0x03;

    /* Set UDP header fields and fill the packet with the data */
    udppkt = (struct udpPkt *)(pkt->curr);
    udppkt->srcPort = hs2net(udpptr->localpt);
    udppkt->dstPort = hs2net(udpptr->remotept);
    udppkt->len = hs2net(pkt->len);
    udppkt->chksum = 0;

    /* Zero out the data buffer so empty space is padded with zeroes */
    bzero(udppkt->data, datalen);
    memcpy(udppkt->data, buf, datalen);

    /* Calculate UDP checksum (which happens to be the same as TCP's) */
    udppkt->chksum = udpChksum(pkt, UDP_HDR_LEN + datalen,
                               &(udpptr->localip), &(udpptr->remoteip));

    /* Send the UDP packet through IP */
    result = ipv4Send(pkt, &(udpptr->localip), &(udpptr->remoteip),
                      IPv4_PROTO_UDP);

    if (SYSERR == netFreebuf(pkt))
    {
        return SYSERR;
    }

    return result;
}
