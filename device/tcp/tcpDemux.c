/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Locate the TCP socket for a TCP packet
 * @param dstpt destination port of the TCP packet
 * @param srcpt source port of the TCP packet
 * @param dstip destination IP of the TCP packet
 * @param srcip source IP of the TCP packet
 * @return most completely matched socket, NULL if no match
 */
struct tcb *tcpDemux(ushort dstpt, ushort srcpt, struct netaddr *dstip,
                     struct netaddr *srcip)
{
    struct tcb *tcbptr = NULL;
    uint i;
    uint level = 0;

    /* Cycle through all tcp devices to find the best match */
    for (i = 0; i < NTCP; i++)
    {
        wait(tcptab[i].mutex);
        if (tcptab[i].state != TCP_CLOSED)
        {
            /* Full match is the best */
            if (level < 3
                && (tcptab[i].localpt == dstpt)
                && (tcptab[i].remotept == srcpt)
                && (netaddrequal(&tcptab[i].localip, dstip))
                && (netaddrequal(&tcptab[i].remoteip, srcip)))
            {
                tcbptr = &tcptab[i];
                level = 3;
                TCP_TRACE("Level 3 match, socket %d", i);
            }

            /* Src and dst ports match */
            if (level < 2
                && (tcptab[i].localpt == dstpt)
                && (tcptab[i].remotept == srcpt)
                && (netaddrequal(&tcptab[i].localip, dstip))
                && (tcptab[i].remoteip.type == NULL))
            {
                tcbptr = &tcptab[i];
                level = 2;
                TCP_TRACE("Level 2 match, socket %d", i);
            }

            /* Dst ports match is last */
            if (level < 1
                && (tcptab[i].localpt == dstpt)
                && (tcptab[i].remotept == NULL)
                && (netaddrequal(&tcptab[i].localip, dstip))
                && (tcptab[i].remoteip.type == NULL))
            {
                tcbptr = &tcptab[i];
                level = 1;
                TCP_TRACE("Level 1 match, socket %d", i);
            }
        }
        signal(tcptab[i].mutex);
    }

    return tcbptr;
}
