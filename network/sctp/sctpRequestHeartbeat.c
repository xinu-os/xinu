/**
 * @file sctpRequestHeartbeat.c
 * @provides sctpRequestHeartbeat.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Request that a remote endpoint send a heartbeat.
 * Reference: RFC 4960, pg. 117.
 * @param *instance Associated SCTP instance
 * @param *remoteaddr Remote network address
 * @param remotept Remote port
 * @return non-zero on error
 */
int sctpRequestHeartbeat(struct sctp *instance, struct netaddr *remoteaddr,
		ushort remotept)
{
    return 0;
}

