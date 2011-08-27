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
	//struct sctp *instance;
	uint checksum, sctp_len;
	int result = 0;

	/* Set up the beginning of the SCTP packet */
	sctp = (struct sctpPkt *)pkt->curr;
	sctp_len = pkt->len - (pkt->curr - pkt->linkhdr);
	checksum = net2hl(sctp->header.checksum); // XXX: need ntohl?
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

    return result;
}

