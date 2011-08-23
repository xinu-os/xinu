/**
 * @file sctpChangeHeartbeat.c
 * @provides sctpChangeHeartbeat.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Change whether a remote endpoint sends a heartbeat or not.
 * Reference: RFC 4960, pg. 116--117.
 * @param *instance Associated SCTP instance
 * @param *remoteaddr Remote network address
 * @param remotept Remote port
 * @param new_state 0 for disable, non-zero for enable
 * @return non-zero on error
 */
int sctpChangeHeartbeat(struct sctp *instance, struct netaddr *remoteaddr,
		ushort remotept, int new_state)
{
    return 0;
}

