/**
 * @file sctpShutdown.c
 * @provides sctpShutdown.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>
#include <mailbox.h>

/**
 * Gracefully shutdown an SCTP association and return an SCTP instance to
 * its pre-associated state. Since Xinu has a 1-1 correspondence between
 * associations and instances this will effectively reset the instance to
 * be ready for re-association.
 * See RFC 4960, pg. 112 for details
 * @param *instance Associated SCTP instance
 * @return result of shutdown process (non-zero for errors)
 */
int sctpShutdown(struct sctp *instance)
{
	int status;

	/* Make sure instance has association */

	/* Make sure all locally queued data is sent for delivery */
	/* state -> SHUTDOWN_PENDING */

	/* Wait for all outstanding data data to be acknowledged */
	/* send SHUTDOWN message */
	/* start shutdown timer */
	/* state -> SHUTDOWN_SENT */

	/* wait for SHUTDOWN or SHUTDOWN ACK message */
	status = mailboxReceive(instance->signal);
	switch (status)
	{
	case SCTP_TYPE_SHUTDOWN_ACK:
		/* stop shutdown timer */
		/* send SHUTDOWN_COMPLETE message */
		break;
	case SCTP_TYPE_SHUTDOWN:
		/* send SHUTDOWN_ACK message */
		/* (re)start shutdown timer */
		/* state -> SHUTDOWN_ACK_SENT */

		/* wait for SHUTDOWN COMPLETE or SHUTDOWN ACK message */
		switch (status)
		{
		case SCTP_TYPE_SHUTDOWN_COMPLETE:
			/* stop shutdown timer */
			break;
		case SCTP_TYPE_SHUTDOWN_ACK:
			/* stop shutdown timer */
			/* send SHUTDOWN_COMPLETE message */
			break;
		default :
			return -1;
		}
		break;
	default:
		/* XXX: better error/cleanup handling */
		return -1;
	}

	/* delete TCB status (reset to pre-associate state) */
	/* state -> CLOSED */

    return 0;
}

