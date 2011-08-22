/**
 * @file sctpStatus.c
 * @provides sctpStatus.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Returns the current status of the SCTP association.  Since all status
 * information is already in the instance, this just returns if the
 * instance has a valid association.
 * @param *instance Associated SCTP instance
 * @return 1 if instance has valid association, 0 otherwise.
 */
int sctpStatus(struct sctp *instance)
{
	/* Make sure instance is valid association */

    return 0;
}

