/**
 * @file sctpAbort.c
 * @provides sctpAbort.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Ungracefully terminate an association. Will return SCTP instance to the
 * initialized state.  (See sctpShutdown details for more information.)
 * Reference: RFC 4960, pg. 113
 * @param *instance SCTP instance association to abort
 * @param reason the user-defined reason for aborting the association
 * @return non-zero on error
 */
int sctpAbort(struct sctp *instance, int reason)
{
	/* Make sure instance has association */

	/* send ABORT message */
	/* delete TCB */
	/* state -> CLOSED */

    return 0;
}

