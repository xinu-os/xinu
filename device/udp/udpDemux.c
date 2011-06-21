/**
 * @file     udpDemux.c
 * @provides udpDemux
 *
 * $Id: udpDemux.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <udp.h>

/**
 * Locate the UDP socket for a UDP packet
 * @param dstpt destination port of the UDP packet
 * @param srcpt source port of the UDP packet
 * @param dstip destination IP of the UDP packet
 * @param srcip source IP of the UDP packet
 * @return most completely matched socket, NULL if no match
 * @pre-condition interrupts are already disabled
 * @post-condition interrupts are still disabled
 */
struct udp *udpDemux(ushort dstpt, ushort srcpt, struct netaddr *dstip,
                     struct netaddr *srcip)
{
    struct udp *udpptr = NULL;
    uint i;
    uint level = 0;

    /* Cycle through all udp devices to find the best match */
    for (i = 0; i < NUDP; i++)
    {
        if (udptab[i].state != UDP_FREE)
        {
            /* Full match is the best */
            if (level < 3
                && (udptab[i].localpt == dstpt)
                && (udptab[i].remotept == srcpt)
                && (netaddrequal(&udptab[i].remoteip, srcip))
                && (netaddrequal(&udptab[i].localip, dstip)))
            {
                udpptr = &udptab[i];
                level = 3;
            }

            /* Src and dst ports match is second */
            if (level < 2
                && (udptab[i].localpt == dstpt)
                && (udptab[i].remotept == srcpt)
                && (netaddrequal(&udptab[i].localip, dstip))
                && (udptab[i].remoteip.type == NULL))
            {
                udpptr = &udptab[i];
                level = 2;
            }

            /* Dst ports match is last */
            if (level < 1
                && (udptab[i].localpt == dstpt)
                && (udptab[i].remotept == NULL)
                && (netaddrequal(&udptab[i].localip, dstip))
                && (udptab[i].remoteip.type == NULL))
            {
                udpptr = &udptab[i];
                level = 1;
            }
        }
    }

    return udpptr;
}
