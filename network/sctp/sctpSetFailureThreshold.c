/**
 * @file sctpSetFailureThreshold.c
 * @provides sctpSetFailureThreshold.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Set the maximum number of retransmissions for a remote endpoint.
 * Reference: RFC 4960, pg. 118.
 * @param *instance Associated SCTP instance
 * @param *remoteaddr Remote network address
 * @param remotept Remote port
 * @param threshold maximum number of retransmissions
 * @return the set number of retransmissions
 */
int sctpSetFailureThreshold(struct sctp *instance, struct netaddr *remoteaddr,
		ushort remotept, uint threshold)
{
    return 0;
}

