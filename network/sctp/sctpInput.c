/**
 * @file sctpInput.c
 * @provides sctpInput.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Accept a packet from the underlying network layer for SCTP processing.
 * @param *pkt Incoming packet
 * @param *src Source network address of packet
 * @param *dst Destination network address of packet
 * @return non-zero on error
 */
int sctpInput(struct packet *pkt, struct netaddr *src, struct netaddr *dst)
{
	struct sctpPkt *sctp;
	struct sctp *instance;
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
	SCTP_TRACE("SCTP Input {%d:%d, 0x%08x}", sctp->header.srcpt,
			   sctp->header.dstpt, sctp->header.tag);

	instance = sctpFindInstance(dst, sctp->header.dstpt);
	if (NULL == instance)
	{
		/* XXX: some special error case here? */
		netFreebuf(pkt);
        SCTP_TRACE("Could not find SCTP instance");
		return result;
	}

	instance = sctpFindAssociation(instance, src, sctp->header.srcpt);
	if (NULL == instance)
	{
		/* XXX: No association, is it INIT packet, should we set one up? */
		netFreebuf(pkt);
        SCTP_TRACE("Could not find SCTP association");
		return result;
	}

	/* lock the TCB */
	wait(instance->lock);

	/* Iterate over the SCTP chunks in the packet */
	pos = sizeof(sctp->header);
	chunk = &sctp->chunk[0];
	while (pos < sctp_len)
	{
		chunk->length = net2hs(chunk->length);
		SCTP_TRACE("Chunk: {%d, 0x%02x, %u}", chunk->type, chunk->flags,
				   chunk->length);

		/* handle this chunk (depends on chunk type and state) */
		switch (instance->state)
		{
		case SCTP_STATE_CLOSED:
			/* XXX: finish this */
			break;
		case SCTP_STATE_COOKIE_WAIT:
			/* XXX: hmmm */
			break;
		case SCTP_STATE_COOKIE_ECHOED:
		case SCTP_STATE_ESTABLISHED:
		case SCTP_STATE_SHUTDOWN_PENDING:
		case SCTP_STATE_SHUTDOWN_SENT:
		case SCTP_STATE_SHUTDOWN_RECEIVED:
		case SCTP_STATE_SHUTDOWN_ACK_SENT:
		}

		/* move to next chunk */
		pos += chunk->length;
		chunk = (struct sctpChunkHeader *)((uchar *)chunk + chunk->length);
	}

	/* unlock the TCB */
	signal(instance->lock);

    return result;
}

/**
 * Given a local IP and local port, find the SCTP instance to which it
 * belongs.
 * @param *localip the local IP of the inbound packet
 * @param localpt the local port of the inbound packet
 * @return the SCTP instance, or NULL if none is found
 */
static struct sctp *sctpFindInstance(struct netaddr *localip, ushort localpt)
{
	int i, j;
	struct sctp *instance;

	for (i = 0; i < NSCTP; i++)
	{
		instance = &sctptab[i];
		for (j = 0; j < SCTP_MAX_IPS; j++)
		{
			if (localpt == instance->localport &&
				netaddrequal(&instalce->localip[j], localip))
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
		struct netaddr *remoteip, ushort remotept)
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

