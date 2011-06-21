/* @file snoopRead.c
 * @provides snoopRead
 *
 * $Id: snoopRead.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <snoop.h>

/**
 * Returns a packet captured from a network interface.
 * @return a packet if read was successful, otherwise SYSERR
 */
struct packet *snoopRead(struct snoop *cap)
{
    struct packet *pkt;

    /* Error check pointers */
    if (NULL == cap)
    {
        return (struct packet *)SYSERR;
    }

    pkt = (struct packet *)mailboxReceive(cap->queue);
    if ((SYSERR == (int)pkt) || (NULL == pkt))
    {
        return (struct packet *)SYSERR;
    }

    return pkt;
}
