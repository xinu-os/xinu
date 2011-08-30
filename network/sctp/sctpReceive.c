/**
 * @file sctpReceive.c
 * @provides sctpReceive.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * Read data from an SCTP association. Non-blocking, if no data is
 * available from `stream_id` this function will return 0.
 * Reference: RFC 4960, pg. 115--116.
 * @param *instance Associated SCTP instance
 * @param buffer receiving data buffer to put message into
 * @param size size of the receive buffer
 * @param stream_id stream identifier to send message on
 * @return number of bytes written to buffer, zero if no data available
 */
int sctpReceive(struct sctp *instance, void *buffer, uint size,
                int stream_id)
{
    /* Make sure instance is valid association */
    /* Read the SCTP data */
    /* Copy the data into the user buffer */

    return 0;
}
