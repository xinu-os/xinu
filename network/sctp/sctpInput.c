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
	uint checksum;
	int result = 0;

	/* Set up the beginning of the SCTP packet */
	sctp = (struct sctpPkt *)pkt->curr;
	sctp_len = pkt->len - (pkt->curr - pkt->nethdr);
	checksum = net2hl(sctp->checksum); // XXX: need ntohl?
	sctp->checksum = 0;
	if (checksum != sctpChecksum(sctp, sctp_len))
	{
		netFreebuf(pkt);
		SCTP_TRACE("Invalid checkum: 0x%08x", checksum);
		return result;
	}

	sctp->srcpt = net2hs(sctp->srcpt);
	sctp->dstpt = net2hs(sctp->dstpt);
	sctp->tag = net2hl(sctp->tag);

	SCTP_TRACE("SCTP Input {%d:%d, 0x%08x}", sctp->srcpt, sctp->dstpt,
			   sctp->tag);

    return result;
}

