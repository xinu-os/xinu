/**
 * @file     udpDemux.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <udp.h>

#define NO_MATCH      0
#define DEST_MATCH    1
#define PARTIAL_MATCH 2
#define FULL_MATCH    3

/**
 * @ingroup udpinternal
 *
 * Locate the UDP socket for a UDP packet
 * @param dstpt destination port of the UDP packet
 * @param srcpt source port of the UDP packet
 * @param dstip destination IP of the UDP packet
 * @param srcip source IP of the UDP packet
 * @return most completely matched socket, NULL if no match
 * @pre-condition interrupts are already disabled
 * @post-condition interrupts are still disabled
 */
struct udp *udpDemux(ushort dstpt, ushort srcpt, const struct netaddr *dstip,
                     const struct netaddr *srcip)
{
    struct udp *udpptr = NULL;
    uint i;
    uint match = NO_MATCH;

    /* Cycle through all udp devices to find the best match */
    for (i = 0; i < NUDP; i++)
    {
        if (udptab[i].state == UDP_FREE)
        {
            /* skip this entry */
            continue;
        }
        if (!netaddrequal(&udptab[i].localip, dstip))
        {
            /* entry is not bound to the destination */
            continue;
        }

        /* Full match is the best */
        if (match < FULL_MATCH
            && (udptab[i].localpt == dstpt)
            && (udptab[i].remotept == srcpt)
            && (netaddrequal(&udptab[i].remoteip, srcip)))
        {
            udpptr = &udptab[i];
            match = FULL_MATCH;
            /* you can't beat a full match */
            break;
        }

        /* Src and dst ports match is second */
        if (match < PARTIAL_MATCH
            && (udptab[i].localpt == dstpt)
            && (udptab[i].remotept == srcpt)
            && (udptab[i].remoteip.type == NULL))
        {
            udpptr = &udptab[i];
            match = PARTIAL_MATCH;
        }

        /* Dst ports match is last */
        if (match < DEST_MATCH
            && (udptab[i].localpt == dstpt)
            && (udptab[i].remotept == NULL)
            && (udptab[i].remoteip.type == NULL))
        {
            udpptr = &udptab[i];
            match = DEST_MATCH;
        }
    }

    return udpptr;
}
