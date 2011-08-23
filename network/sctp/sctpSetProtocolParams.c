/**
 * @file sctpSetProtocolParams.c
 * @provides sctpSetProtocolParams.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Customize some protocol parameters
 * Reference: RFC 4960, pg. 118--119.
 * @param *instance Associated SCTP instance
 * @param param_id parameter to change
 * @param param_value new value of parameter
 * @return non-zero on error
 */
int sctpSetProtocolParams(struct sctp *instance, int param_id, int param_value)
{
    return 0;
}

