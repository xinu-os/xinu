/**
 * @file sctpAssociate.c
 * @provides sctpAssociate.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <mailbox.h>
#include <sctp.h>

/**
 * Associate an SCTP instance with another endpoint. `instance` must be a
 * valid SCTP instance.  `remoteaddr` and `remotept` define the destination
 * transport address. `nos` defines the number of output streams requested.
 * NOTE: Xinu has a 1-1 correspondence between SCTP instances and
 * associations.
 * NOTE: This is a BLOCKING call, on return the SCTP instance will be
 * populated with all possible remote addresses and nos may be less than
 * the requested amount. The user must confirm that this is sufficient.
 * See RFC 4960, pg. 111--112.
 * @param *instance Initialized SCTP instance
 * @param *remoteaddr Destination network address
 * @param remotept Destination transport protocol port
 * @param nos Number of output streams
 * @return NULL if there is an error, `instance` on success
 */
struct sctp *sctpAssociate(struct sctp *instance, struct netaddr *remoteaddr,
                           int remotept, int nos)
{
	int status;

	/* Make sure instance is valid */
	if (NULL == instance || SCTP_DEV_FREE == instance->dev_state)
	{
		return NULL;
	}

	/* "create" TCB (already allocated durning Initialize) */

	/* send INIT message */

	/* start init timer for timeout */
	/* state -> COOKIE_WAIT */
	instance->state = SCTP_STATE_COOKIE_WAIT;

	/* wait for INIT ACK */
	status = mailboxReceive(instance->signal);
	if (status != SCTP_TYPE_INIT_ACK)
	{
		SCTP_TRACE("Received bad status: %d", status);
	}
	/* send COOKIE_ECHO message */
	/* stop init timer */
	/* start cookie timer */
	/* state -> COOKIE_ECHOED */
	instance->state = SCTP_STATE_COOKIE_ECHOED;

	/* wait for COOKIE ACK */
	status = mailboxReceive(instance->signal);
	if (status != SCTP_TYPE_COOKIE_ACK)
	{
		SCTP_TRACE("Received bad status: %d", status);
	}
	/* stop cookie timer */
	/* state -> ESTABLISHED */
	instance->state = SCTP_STATE_ESTABLISHED;

    return instance;
}

