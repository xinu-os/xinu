/**
 * @file sctpInitialize.c
 * @provides sctpInitialize.
 *
 * $Id$
 */

#include <stddef.h>
#include <network.h>
#include <sctp.h>

#ifndef NSCTP
# define NSCTP 0
#endif

struct sctp sctptab[NSCTP];

static struct sctp sctpGetInstance(void);
static int sctpGetPort(void);

/**
 * Allocate and initialize an SCTP instance. If `localpt` is 0, a port
 * number will be assigned dynamically.  Since SCTP can be multi-homed
 * localaddr is a pointer to `naddrs` local IP addresses to support.
 * If `localaddr` is NULL or `naddrs` is 0, the "primary" IP used will be
 * the first local IP used with this instance.
 * @param localpt local port for this SCTP instance
 * @param *localaddr pointer to start of struct netaddr array
 * @param naddrs number of localaddrs in array
 * @return SCTP instance, NULL on error
 */
struct sctp sctpInitialize(int localpt, struct netaddr *localaddr, int naddrs)
{
	struct sctp instance;
	int i;

	/* get the instance as needed */
	instance = sctpGetInstance();
	if (NULL == instance)
	{
		SCTP_TRACE("Failed to find available SCTP instance");
		return NULL;
	}

	wait(instance->lock);

	/* set local port for instance */
	if (0 == localpt)
	{
		localpt = sctpGetPort();
	}
	instance->localport = localpt;
	SCTP_TRACE("SCTP has port %d", instance->localport);

	/* set local IPs (max of SCTP_MAX_IPS) */
	if (naddrs > SCTP_MAX_IPS)
	{
		naddrs = SCTP_MAX_IPS;
	}
	for (i = 0; i < naddrs; i++)
	{
		memcpy(&instance->localip[i], &localaddr[i], sizeof(localaddr[i]));
	}

	signal(instance->lock);
	SCTP_TRACE("SCTP instance initialized with %d homes", naddrs);

    return instance;
}

/**
 * Find and allocate a free SCTP instance.
 * @return pointer to SCTP instance allocated (NULL if none available)
 */
static struct sctp sctpGetInstance(void)
{
	int i;
	struct sctp instance = NULL;

	for (i = 0; i < NSCTP; i++)
	{
		instance = &sctptab[i];
		wait(instance->lock);
		if (SCTP_DEV_FREE == instance->dev_state)
		{
			instance->dev_state = SCTP_DEV_USED;
			signal(instance->lock);
			break;
		}
		signal(instance->lock);
	}
	return instance;
}

/**
 * Find and allocate an available SCTP port
 * @return SCTP port
 */
static int sctpGetPort(void)
{
	static int nextport = -1;
	int i;

	/* XXX: pretty sure there is a race condition on nextport */
	nextport = (nextport + 1) % (SCTP_HIGHPORT - SCTP_LOWPORT + 1);
	for (i = 0; i < NSCTP; i++)
	{
		if (sctptcb[i].localpt == (SCTP_LOWPORT + port))
		{
			nextport = (nextport + 1) % (SCTP_HIGHPORT - SCTP_LOWPORT + 1);
			i = 0;
		}
	}

	return (SCTP_LOWPORT + nextport);
}

