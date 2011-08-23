/**
 * @file sctpGetSRTTReport.c
 * @provides sctpGetSRTTReport.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Report the current Smoothed Round-Trip Time between the local endpoint
 * and a remote endpoint.
 * Reference: RFC 4960, pg. 117--118.
 * @param *instance Associated SCTP instance
 * @param *remoteaddr Remote network address
 * @param remotept Remote port
 * @return the SRTT (in milliseconds)
 */
int sctpGetSRTTReport(struct sctp *instance, struct netaddr *remoteaddr,
		ushort remotept)
{
    return 0;
}

