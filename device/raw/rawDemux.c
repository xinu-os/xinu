/**
 * @file rawDemux.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <raw.h>

/**
 * @ingroup raw
 *
 * Locate the raw socket for a packet
 * @param src source IP address of the packet
 * @param dst destination IP address of the packet
 * @param proto protocol of the packet
 * @return most completely matched socket, NULL if no match
 */
struct raw *rawDemux(struct netaddr *src, struct netaddr *dst,
                     ushort proto)
{
    struct raw *rawptr;
    uint i;
    uint level;

    rawptr = NULL;
    level = 0;

#ifdef TRACE_RAW
    char strA[20], strB[20];
    netaddrsprintf(strA, src);
    netaddrsprintf(strB, dst);
    RAW_TRACE("Demultiplex proto %d src %s dst %s", proto, strA, strB);
#endif

    /* Cycle through all RAW devices to find the best match */
    for (i = 0; i < NRAW; i++)
    {
        if (RAW_ALLOC == rawtab[i].state)
        {
#ifdef TRACE_RAW
            netaddrsprintf(strA, &rawtab[i].localip);
            netaddrsprintf(strB, &rawtab[i].remoteip);
            RAW_TRACE("Socket %d proto %d local IP %s remote IP %s", i,
                      rawtab[i].proto, strA, strB);
#endif

            /* Full match is the best */
            if (level < 4
                && (proto == rawtab[i].proto)
                && (netaddrequal(src, &rawtab[i].remoteip))
                && (netaddrequal(dst, &rawtab[i].localip)))
            {
                rawptr = &rawtab[i];
                level = 4;
                RAW_TRACE("Level 4 match, socket %d", i);
            }

            /* Protocol and remote IP match is second */
            if (level < 3
                && (proto == rawtab[i].proto)
                && (((netaddrequal(src, &rawtab[i].remoteip))
                     && (NULL == rawtab[i].localip.type))
                    || ((netaddrequal(dst, &rawtab[i].localip))
                        && (NULL == rawtab[i].remoteip.type))))
            {
                rawptr = &rawtab[i];
                level = 3;
                RAW_TRACE("Level 3 match, socket %d", i);
            }

            /* Protocol match is third */
            if (level < 2
                && (proto == rawtab[i].proto)
                && (NULL == rawtab[i].remoteip.type)
                && (NULL == rawtab[i].localip.type))
            {
                rawptr = &rawtab[i];
                level = 2;
                RAW_TRACE("Level 2 match, socket %d", i);
            }

            /* All protocols is last */
            if (level < 1
                && (NULL == rawtab[i].proto)
                && (NULL == rawtab[i].remoteip.type)
                && (NULL == rawtab[i].localip.type))
            {
                rawptr = &rawtab[i];
                level = 1;
                RAW_TRACE("Level 1 match, socket %d", i);
            }

        }
    }

    return rawptr;
}
