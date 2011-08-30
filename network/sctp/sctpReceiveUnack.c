/**
 * @file sctpReceiveUnack.c
 * @provides sctpReceiveUnack.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * NOT IMPLEMENTED, Retrieve a message that was never acknowledged.
 * Reference: RFC 4960, pg. 119--120.
 * @param data_id identifier for message (as given by failure notification)
 * @param *buffer buffer for receiving data
 * @param size size of data buffer
 * @return number of bytes in buffer
 */
int sctpReceiveUnack(int data_id, void *buffer, uint size, int stream_id)
{
    return 0;
}
