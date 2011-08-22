/**
 * @file sctpSetPrimary.c
 * @provides sctpSetPrimary.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Set the primary remote destination for this SCTP association.
 * Reference: RFC 4960, pg. 114--115.
 * @param *instance Associated SCTP instance
 * @param *remoteaddr Destination network address
 * @param remotept Destination transport protocol port
 * @return non-zero on error
 */
int sctpSetPrimary(struct sctp *instance, struct netaddr *remoteaddr,
				  int remotept)
{
	/* Make sure SCTP instance is associated */

	/* make sure remoteaddr:remotept are valid remotes */
	/* swap 0th transport entry with index of this entry */

    return 0;
}

