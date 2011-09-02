/**
 * @file sctpInputInit.c
 * @provides sctpInputInit.
 *
 * $Id$
 */

#include <stddef.h>
#include <stdlib.h>
#include <sctp.h>
#include <clock.h>

/**
 * SCTP input INIT handler. Given an INIT chunk, process it and ready a
 * acknowledgement for association establishment.
 * @param *instance SCTP instance the chunk came in on
 * @param *init_chunk chunk received with init request
 * @return non-zero on error
 */
int sctpInputInit(struct sctp *instance, struct sctpChunkInit *init_chunk,
                  struct netaddr *src, struct sctpPkt *sctp)
{
    /* Someone is trying to connect */
    struct sctpChunkInitAck *initack_chunk;
    struct sctpCookie *cookie;
    int result;
    uint length = sizeof(*initack_chunk) + sizeof(*cookie);

    /* Set up response chunk */
    initack_chunk = memget(length);
    if ((void *)SYSERR == initack_chunk)
    {
        return SYSERR;
    }
    initack_chunk->head.type = SCTP_TYPE_INIT_ACK;
    initack_chunk->head.flags = 0;
    initack_chunk->head.length = hs2net(length);
    initack_chunk->init_tag = hl2net(rand());
    initack_chunk->a_rwnd = hl2net(1600); // XXX: Figure out this value
    initack_chunk->n_out_streams = hs2net(SCTP_MAX_STREAMS);
    initack_chunk->n_in_streams = hs2net(SCTP_MAX_STREAMS);
    initack_chunk->init_tsn = hl2net(0); // XXX: Figure out this value

    /* Generate Cookie Param -- DO NOT make TCB */
    initack_chunk->param[0].type =
        hs2net(SCTP_INITACK_PARAM_STATE_COOKIE);
    initack_chunk->param[0].length =
        hs2net(sizeof(initack_chunk->param[0])+sizeof(*cookie));
    cookie = (struct sctpCookie *)&initack_chunk->param[1];
    cookie->create_time = clktime;
    cookie->life_time = 10;     /* XXX: Valid.Cookie.Life */
    memcpy(&cookie->remoteip, src, sizeof(cookie->remoteip));
    cookie->remotept = sctp->header.srcpt;
    cookie->peer_tag = net2hl(init_chunk->init_tag);
    cookie->my_tag = initack_chunk->init_tag;
    cookie->mac = 0;
    cookie->mac = sctpCookieDigest(instance->secret, cookie);

    /* Send INIT ACK */
    result = sctpOutput(instance, initack_chunk, length);
    memfree(initack_chunk, length);

    return result;
}
