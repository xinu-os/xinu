/**
 * @file sctpInputCookieEcho.c
 * @provides sctpInputCookieEcho.
 *
 * $Id$
 */

#include <stddef.h>
#include <sctp.h>
#include <clock.h>

/**
 * SCTP input COOKIE ECHO handler. Given a COOKIE ECHO chunk, process it and
 * ready a acknowledgement.  Should create a valid association.
 * @param *init_chunk chunk received with init request
 * @return non-zero on error
 */
int sctpInputCookieEcho(struct sctp *instance,
                        struct sctpChunkCookieEcho *cecho_chunk)
{
    /* Someone is trying to *legitimately* connect */
    struct sctpChunkCookieAck cack_chunk;
    struct sctpCookie *cookie;
    uint mac;

    /* Validate Cookie */
    cookie = (struct sctpCookie *)cecho_chunk->cookie;
    mac = cookie->mac;
    cookie->mac = 0;
    if (sctpCookieDigest(instance->secret, cookie) != mac)
    {
        SCTP_TRACE("COOKIE ECHO returned invalid cookie");
        return SYSERR;
    }

    /* Check create time/life time of cookie */
    if (cookie->create_time + cookie->life_time < clktime)
    {
        SCTP_TRACE("COOKIE ECHO returned after life time expired");
        return SYSERR;
    }

    /* Make sure instance can accept an association */
    wait(instance->lock);
    if (SCTP_STATE_CLOSED != instance->state)
    {
        SCTP_TRACE("COOKIE ECHO cannot use non-closed association");
        signal(instance->lock);
        return SYSERR;
    }

    /* Create TCB for association */
    instance->state = SCTP_STATE_ESTABLISHED;
    instance->my_tag = cookie->my_tag;
    instance->peer_tag = cookie->peer_tag;
    instance->remoteport = cookie->remotept;
    memcpy(&instance->remoteip[0], &cookie->remoteip, sizeof(cookie->remoteip));
    instance->primary_path = 0;
    // XXX: Probably need more...
    signal(instance->lock);

    /* Send COOKIE ACK */
    cack_chunk.head.type = SCTP_TYPE_COOKIE_ACK;
    cack_chunk.head.flags = 0;
    cack_chunk.head.length = hs2net(sizeof(cack_chunk));
    sctpOutput(instance, &cack_chunk, sizeof(cack_chunk));

    return OK;
}
