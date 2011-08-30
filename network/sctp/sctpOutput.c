/**
 * @file sctpOutput.c
 * @provides sctpOutput.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <ipv4.h>
#include <sctp.h>

/**
 * Given an instance, data, and data length output a packet to the network
 * layer for sending.
 * @param *instance Associated SCTP instance
 * @param *data data that will be transferred
 * @param length length of chunk
 * @return non-zero on error
 */
int sctpOutput(struct sctp *instance, void *data, uint length)
{
    int result;
    struct sctpHeader *header;
    struct sctpChunkParam *param;
    struct sctpCookie *cookie;
    struct packet *pkt;
    struct netaddr *localip, *remoteip;
    ushort localpt, remotept;

    /* Make sure instance is valid */
    if (SCTP_DEV_USED != instance->dev_state)
    {
        SCTP_TRACE("Invalid instance given");
        return SYSERR;
    }
    localip = &instance->localip[0];
    localpt = instance->localport;

    /* Check if we have an association (or close to it) */
    if (SCTP_STATE_CLOSED != instance->state)
    {
        remoteip = &instance->remoteip[0];
        remotept = instance->remoteport;
    }
    else if (SCTP_TYPE_INIT_ACK == ((struct sctpChunkHeader *)data)->type)
    {
        param = ((struct sctpChunkInitAck *)data)->param;
        /* For simplicity, we assume the cookie is the first param */
        if (SCTP_INITACK_PARAM_STATE_COOKIE != param->type)
        {
            SCTP_TRACE("Missing cookie param");
            return SYSERR;
        }
        cookie = (struct sctpCookie *)param->value;
        remoteip = &cookie->remoteip;
        remotept = cookie->remotept;
    }
    else
    {
        SCTP_TRACE("No valid association, or cookie");
        return SYSERR;
    }

    /* acquire a netbuf */
    pkt = netGetbuf();
    if (SYSERR == (int)pkt)
    {
        SCTP_TRACE("Failed to get netbuf");
        return SYSERR;
    }

    /* Data should be chunked, we just add the common header */
    pkt->curr -= (length + 0x03) & ~0x03;       // 4-byte aligned
    pkt->len = length;

    /* copy sctp chunk into netbuf */
    memcpy(pkt->curr, data, length);

    /* Back off for SCTP header */
    pkt->curr -= sizeof(*header);
    pkt->len += sizeof(*header);
    header = (struct sctpHeader *)pkt->curr;
    header->srcpt = localpt;
    header->dstpt = remotept;
    header->tag = 0;
    header->checksum = 0;
    header->checksum = sctpChecksum(pkt->curr, pkt->len);

    /* Bump over to network layer */
    switch (localip->type)
    {
    case NETADDR_IPv4:
        result = ipv4Send(pkt, localip, remoteip, IPv4_PROTO_SCTP);
        break;
    default:
        result = SYSERR;
        break;
    }

    if (OK == result)
    {
        SCTP_TRACE("Sent %d:%d ; 0x%08x", localpt, remotept, header->tag);
    }
    else
    {
        SCTP_TRACE("Send failure on %d:%d ; 0x%08x",
                   localpt, remotept, header->tag);
    }

    if (SYSERR == netFreebuf(pkt))
    {
        result = SYSERR;
    }

    return result;
}
