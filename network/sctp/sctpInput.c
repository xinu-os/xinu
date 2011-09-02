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

int sctpInputInit(struct sctp *, struct sctpChunkInit *, struct netaddr *,
                  struct sctpPkt *);
int sctpInputCookieEcho(struct sctp *, struct sctpChunkCookieEcho *);
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
    uint checksum, sctp_len, pos = 0;
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
            SCTP_TRACE("No SCTP association, got INIT chunk");

            /* Process INIT and send back INIT ACK */
            result = sctpInputInit(instance, (struct sctpChunkInit *)chunk,
                                   src, sctp);

            /* No more chunks allowed with INIT, clean up */
            netFreebuf(pkt);
            return result;
        }
        else if (SCTP_TYPE_COOKIE_ECHO == chunk->type)
        {
            SCTP_TRACE("No SCTP association, COOKIE ECHO chunk");

            /* Process and send COOKIE ACK, get new association id */
            result = sctpInputCookieEcho(instance,
                                         (struct sctpChunkCookieEcho *)chunk);
            assoc = sctpFindAssociation(instance, src, sctp->header.srcpt);
            if (0 != result || NULL == assoc)
            {
                netFreebuf(pkt);
                return result;
            }

            /* Cookie Echo can come with more chunks */
            chunk->length = SCTP_PAD(net2hs(chunk->length));
            pos += chunk->length;
            chunk =
                (struct sctpChunkHeader *)((uchar *)chunk + chunk->length);
        }
        else
        {
            /* XXX: No association, not INIT or COOKIE ECHO what should we do? */
            SCTP_TRACE("Could not find SCTP association");
            netFreebuf(pkt);
            return result;
        }
    }

    /* lock the TCB */
    wait(assoc->lock);

    /* Iterate over the SCTP chunks in the packet */
    pos += sizeof(sctp->header);
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
            break;
        case SCTP_STATE_ESTABLISHED:
            break;
        case SCTP_STATE_SHUTDOWN_PENDING:
        case SCTP_STATE_SHUTDOWN_SENT:
        case SCTP_STATE_SHUTDOWN_RECEIVED:
        case SCTP_STATE_SHUTDOWN_ACK_SENT:
            break;
        }

        /* move to next chunk */
        chunk->length = SCTP_PAD(chunk->length);
        pos += chunk->length;
        chunk = (struct sctpChunkHeader *)((uchar *)chunk + chunk->length);
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
        wait(instance->lock);
        for (j = 0; j < SCTP_MAX_IPS; j++)
        {
            if (localpt == instance->localport &&
                netaddrequal(&instance->localip[j], localip))
            {
                signal(instance->lock);
                return instance;
            }
        }
        signal(instance->lock);
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

    wait(instance->lock);
    if (instance->remoteport != remotept)
    {
        signal(instance->lock);
        return NULL;
    }

    for (i = 0; i < SCTP_MAX_IPS; i++)
    {
        if (netaddrequal(&instance->remoteip[i], remoteip))
        {
            signal(instance->lock);
            return instance;
        }
    }
    signal(instance->lock);

    return NULL;
}

