/**
 * @file sctpDestroy.c
 * @provides sctpDestroy.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Destroy and free an SCTP instance.
 * Reference: RFC 4960, pg. 120.
 * @param *instance Valid SCTP instance
 * @return non-zero on error
 */
int sctpDestroy(struct sctp *instance)
{
    /* Make sure it is a valid instance */

    /* clear out port and addresses */

    /* free the "device" */
    instance->dev_state = SCTP_DEV_FREE;

    return 0;
}
