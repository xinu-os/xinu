/**
 * @file sctpInput.c
 * @provides sctpInput.
 *
 * $Id$
 */

#include <stddef.h>
#include <stdlib.h>
#include <network.h>
#include <sctp.h>
#include <clock.h>

static struct sctp *sctpFindAssociation(struct sctp *, struct netaddr *,
                                        ushort);
static struct sctp *sctpFindInstance(struct netaddr *, ushort);

/**
 * Accept a packet from the underlying network layer for SCTP processing.
 * @param *pkt Incoming packet
 * @param *src Source network address of packet
 * @param *dst Destination network address of packet
 * @return non-zero on error
 */
int sctpInput(struct packet *pkt, struct netaddr *src,
              struct netaddr *dst)
{
    struct sctpPkt *sctp;
    struct sctp *instance, *assoc;
    struct sctpChunkHeader *chunk;
    uint checksum, sctp_len, pos;
    int result = 0;

    /* Set up the beginning of the SCTP packet */
    sctp = (struct sctpPkt *)pkt->curr;
    sctp_len = pkt->len - (pkt->curr - pkt->linkhdr);
    checksum = net2hl(sctp->header.checksum);
    sctp->header.checksum = 0;
    if (checksum != sctpChecksum(sctp, sctp_len))
    {
        netFreebuf(pkt);
        SCTP_TRACE("Invalid checkum: 0x%08x != 0x%08x", checksum,
                   sctpChecksum(sctp, sctp_len));
        return result;
    }

    sctp->header.srcpt = net2hs(sctp->header.srcpt);
    sctp->header.dstpt = net2hs(sctp->header.dstpt);
    sctp->header.tag = net2hl(sctp->header.tag);
    chunk = &sctp->chunk[0];
    SCTP_TRACE("SCTP Input {%d:%d, 0x%08x, Chunk:%d}", sctp->header.srcpt,
               sctp->header.dstpt, sctp->header.tag, chunk->type);

    instance = sctpFindInstance(dst, sctp->header.dstpt);
    if (NULL == instance)
    {
        /* XXX: some special error case here? */
        netFreebuf(pkt);
        SCTP_TRACE("Could not find SCTP instance");
        return result;
    }

    assoc = sctpFindAssociation(instance, src, sctp->header.srcpt);
    if (NULL == assoc)
    {
        /* Should only be CLOSED if no association TCB exists */
        if (SCTP_TYPE_INIT == chunk->type)
        {
            /* Someone is trying to connect */
            struct sctpChunkInitAck *initack_chunk;
            struct sctpCookie *cookie;
            struct sctpChunkInit *init_chunk = (struct sctpChunkInit *)chunk;
            uint length = sizeof(*initack_chunk) + sizeof(*cookie) - 1;

            SCTP_TRACE("No SCTP association, INIT chunk");

            /* Set up response chunk */
            initack_chunk = memget(length);
            initack_chunk->head.type = SCTP_TYPE_INIT_ACK;
            initack_chunk->head.flags = 0;
            initack_chunk->head.length = hs2net(length);
            initack_chunk->init_tag = hl2net(rand());
            initack_chunk->a_rwnd = hl2net(1600); // XXX: Figure out this value
            initack_chunk->n_out_streams = hs2net(SCTP_MAX_STREAMS);
            initack_chunk->n_in_streams = hs2net(SCTP_MAX_STREAMS);
            initack_chunk->init_tsn = hl2net(0); // XXX: Figure out this value

            /* Generate Cookie Param-- DO NOT make TCB */
            initack_chunk->param[0].type =
                hs2net(SCTP_INITACK_PARAM_STATE_COOKIE);
            initack_chunk->param[0].length =
                hs2net(sizeof(initack_chunk->param[0])+sizeof(*cookie)-1);
            cookie = (struct sctpCookie *)initack_chunk->param[0].value;
            cookie->create_time = clktime;
            cookie->life_time = 10;     /* Valid.Cookie.Life */
            memcpy(&cookie->remoteip, src, sizeof(cookie->remoteip));
            cookie->remotept = sctp->header.srcpt;
            cookie->peer_tag = net2hl(init_chunk->init_tag);
            cookie->mac = 0;
            //cookie->mac = sctpCookieDigest(instance->secret, cookie);
            /* Send INIT ACK */
            sctpOutput(instance, initack_chunk, length);
        }
        else if (SCTP_TYPE_COOKIE_ECHO == chunk->type)
        {
            /* Someone is trying to *legitimately* connect */
            /* Validate Cookie */
            /* Create TCB for association */
            /* Send COOKIE ACK */
            SCTP_TRACE("No SCTP association, COOKIE ECHO chunk");
        }
        else
        {
            /* XXX: No association, not INIT or COOKIE ECHO what should we do? */
            SCTP_TRACE("Could not find SCTP association");
        }
        netFreebuf(pkt);
        return result;
    }

    /* lock the TCB */
    wait(assoc->lock);

    /* Iterate over the SCTP chunks in the packet */
    pos = sizeof(sctp->header);
    while (pos < sctp_len)
    {
        chunk->length = net2hs(chunk->length);
        SCTP_TRACE("Chunk: {%d, 0x%02x, %u}", chunk->type, chunk->flags,
                   chunk->length);

        /* handle this chunk (depends on chunk type and state) */
        switch (assoc->state)
        {
        case SCTP_STATE_CLOSED:
            /* XXX: Should never hit this case, error */
            break;
        case SCTP_STATE_COOKIE_WAIT:
            /* Should handle INIT ACK (or ABORT) */
            break;
        case SCTP_STATE_COOKIE_ECHOED:
            /* Should handle COOKIE ACK (or ABORT) */
        case SCTP_STATE_ESTABLISHED:
        case SCTP_STATE_SHUTDOWN_PENDING:
        case SCTP_STATE_SHUTDOWN_SENT:
        case SCTP_STATE_SHUTDOWN_RECEIVED:
        case SCTP_STATE_SHUTDOWN_ACK_SENT:
            break;
        }

        /* move to next chunk */
        pos += chunk->length;
        chunk =
            (struct sctpChunkHeader *)((uchar *)chunk + chunk->length);
    }

    /* unlock the TCB */
    signal(assoc->lock);

    return result;
}

/**
 * Given a local IP and local port, find the SCTP instance to which it
 * belongs.
 * @param *localip the local IP of the inbound packet
 * @param localpt the local port of the inbound packet
 * @return the SCTP instance, or NULL if none is found
 */
static struct sctp *sctpFindInstance(struct netaddr *localip,
                                     ushort localpt)
{
    int i, j;
    struct sctp *instance;

    for (i = 0; i < NSCTP; i++)
    {
        instance = &sctptab[i];
        for (j = 0; j < SCTP_MAX_IPS; j++)
        {
            if (localpt == instance->localport &&
                netaddrequal(&instance->localip[j], localip))
            {
                return instance;
            }
        }
    }

    return NULL;
}

/**
 * Given a remote IP and remote port find the "association" in the instance
 * to which this packet belongs.
 * NOTE: Since Xinu has a 1-1 correspondence between instances and
 * associations, this just confirms that the remote endpoint belongs to
 * this instance.
 * @param *instance the SCTP instance to check
 * @param *remoteip the remote IP of the inbound packet
 * @param remotept the remote port of the inbound packet
 * @return the SCTP "association", or NULL if none is found
 */
static struct sctp *sctpFindAssociation(struct sctp *instance,
                                        struct netaddr *remoteip,
                                        ushort remotept)
{
    int i;

    if (instance->remoteport != remotept)
    {
        return NULL;
    }

    for (i = 0; i < SCTP_MAX_IPS; i++)
    {
        if (netaddrequal(&instance->remoteip[i], remoteip))
        {
            return instance;
        }
    }

    return NULL;
}
