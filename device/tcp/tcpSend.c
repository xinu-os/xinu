/**
 * @file tcpSend.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Sends a TCP packet for a TCP connection.
 * @param tcbptr pointer to the transmission control block for connection
 * @param ctrl control flags to be set
 * @param seqnum sequence number for the packet
 * @param acknum acknowledgement number for the packet
 * @param data data to be included in the packet, NULL if no data
 * @param datalen length of the data, 0 if no data
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
int tcpSend(struct tcb *tcbptr, uchar ctrl, uint seqnum,
            uint acknum, uint datastart, ushort datalen)
{
    struct packet *pkt = NULL;
    struct tcpPkt *tcp = NULL;
    int result;
    uchar *data;
    uint i = 0;
    ushort window = 0;
    ushort msslen = 0;
    ushort tcplen;

    /* If SYN is set, then don't include in datalen, but include MSS */
    if (ctrl & TCP_CTRL_SYN)
    {
        datalen--;
        msslen = ((3 + TCP_OPT_MSS_SIZE) & ~0x03);
        TCP_TRACE("No SYN in datalen, include MSS");
    }
    /* If FIN is set, then don't include in datalen */
    if (ctrl & TCP_CTRL_FIN)
    {
        datalen--;
        TCP_TRACE("No FIN in datalen");
    }

    /* Get space to construct packet */
    tcplen = TCP_HDR_LEN + datalen + msslen;
    if (tcplen > NET_MAX_PKTLEN)
    {
        TCP_TRACE("Packet too large");
        return SYSERR;
    }
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        TCP_TRACE("Failed to get buffer");
        return SYSERR;
    }

    /* Back off end of buffer to add TCP packet, preserving word alignment */
    pkt->curr -= (tcplen + 0x7) & ~0x7;
    pkt->len = tcplen;

    /* Set TCP header fields */
    tcp = (struct tcpPkt *)pkt->curr;
    tcp->srcpt = tcbptr->localpt;
    tcp->dstpt = tcbptr->remotept;
    tcp->seqnum = seqnum;
    tcp->acknum = acknum;
    tcp->offset = octets2offset(TCP_HDR_LEN + msslen);
    tcp->control = ctrl;
    tcp->window = tcpSendWindow(tcbptr);
    window = tcp->window;
    data = tcp->data;

    /* Add options */
    if (msslen)
    {
        *data++ = TCP_OPT_MSS;
        *data++ = TCP_OPT_MSS_LEN;
        *((ushort *)data) = hs2net((tcbptr->rcvmss + TCP_HDR_LEN));
        data += TCP_OPT_MSS_LEN;
        for (i = (TCP_OPT_MSS_LEN % 4); i > 1; i--)
        {
            *data++ = TCP_OPT_NOP;
        }
        *data++ = TCP_OPT_END;
        TCP_TRACE("Added MSS");
    }

    /* Copy data into packet */
    if (datalen > 0)
    {
        for (i = 0; i < datalen; i++)
        {
            *data++ = tcbptr->out[(datastart + i) % TCP_OBLEN];
        }
    }

    /* Convert TCP header fields to net order */
    tcp->srcpt = hs2net(tcp->srcpt);
    tcp->dstpt = hs2net(tcp->dstpt);
    tcp->seqnum = hl2net(tcp->seqnum);
    tcp->acknum = hl2net(tcp->acknum);
    tcp->window = hs2net(tcp->window);
    tcp->urgent = hs2net(tcp->urgent);

    /* Calculate TCP checksum */
    tcp->chksum = tcpChksum(pkt, tcplen, &tcbptr->localip,
                            &tcbptr->remoteip);

    /* Send TCP packet */
    result = ipv4Send(pkt, &tcbptr->localip, &tcbptr->remoteip,
                      IPv4_PROTO_TCP);

    if (SYSERR == netFreebuf(pkt))
    {
        return SYSERR;
    }

    if (result == OK)
    {
    TCP_TRACE("SENT <C=0x%02X><S=%u><A=%u><dl=%u><w=%u>",
                  ctrl, seqnum, acknum, datalen, window)}
    else
    {
        TCP_TRACE("FAILED to send <C=0x%02X><S=%u><A=%u><dl=%u><w=%u>",
                  ctrl, seqnum, acknum, datalen, window);
    }

    return result;
}
