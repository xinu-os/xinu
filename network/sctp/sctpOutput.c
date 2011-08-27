/**
 * @file sctpOutput.c
 * @provides sctpOutput.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

/**
 * @param *instance Associated SCTP instance
 * @param type type of message to encapsulate
 * @param flags flags to be transmitted in the chunk
 * @param length length of chunk
 * @param *data data that will be transferred
 * @return non-zero on error
 */
int sctpOutput(struct sctp *instance, uchar type, uchar flags,
		       ushort length, void *data)
{
	/* Make sure instance has association */

	/* acquire a netbuf */
	/* create the sctpChunk (if needed) */
	/* copy sctp chunk into netbuf */

    return 0;
}

