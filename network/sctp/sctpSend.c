/**
 * @file sctpSend.c
 * @provides sctpSend.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Send user data via SCTP.  Non-blocking, but data will be copied out of
 * buffer before function returns.
 * Reference: RFC 4960, pg. 113--114.
 * @param *instance Associated SCTP instance
 * @param buffer data buffer containing message to send
 * @param size size of the buffer to send
 * @param stream_id stream identifier to send message on
 * @return non-zero on error
 */
int sctpSend(struct sctp *instance, void *buffer, uint size,
             int stream_id)
{
    /* Make sure instance is valid association */
    /* Ready the SCTP data chunk for sending */
    /* Send the data chunk */

    return 0;
}
