/**
 * @file sctpCookieDigest.c
 * @provides sctpCookieDigest.
 *
 * $Id$
 */

#include <stddef.h>
#include <sctp.h>

/**
 * XXX: This should be a good crypto routine; it is not.
 * @param secret instance specific secret key
 * @param *cookie pointer to the cookie to digest
 * @return result of digest operation
 */
uint sctpCookieDigest(uint secret, struct sctpCookie *cookie)
{
    uint i, temp;
    uchar *c =(uchar *)cookie;
    uint len = sizeof(*cookie);
    uint result = 0xdeadc0de + secret;

    for (i = 0; i < len; i += 4)
    {
        temp = (c[i] << 24) | (c[i+2] << 16) | (c[i+1] << 8) | c[i+3];
        result ^= temp;
    }

    return result;
}
