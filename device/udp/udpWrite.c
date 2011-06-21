/**
 * @file     udpWrite.c
 * @provides udpWrite
 *
 * $Id: udpWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <udp.h>

/**
 * Tell a UDP device to send a packet with the specified data
 * @param devptr UDP device table entry
 * @param buf Buffer of data to be sent
 * @param len Length of data to be sent
 * @return OK if the packet was sent, otherwise SYSERR
 */
devcall udpWrite(device *devptr, void *buf, uint len)
{
    struct udp *udpptr;
    uchar *p = (uchar *)buf;

    udpptr = &udptab[devptr->minor];

    /* Check if we have a specified remote port and ip */
    if (NULL == udpptr->remotept || NULL == udpptr->remoteip.type)
    {
        return SYSERR;
    }

    /* Carry out the actual writing */
    while (len > 0)
    {
        if (len > UDP_MAX_DATALEN)
        {
            if (SYSERR == udpSend(udpptr, UDP_MAX_DATALEN, p))
            {
                return SYSERR;
            }
            len -= UDP_MAX_DATALEN;
            p += UDP_MAX_DATALEN;
        }
        else
        {
            if (SYSERR == udpSend(udpptr, len, p))
            {
                return SYSERR;
            }
            len = 0;
        }
    }

    return len;
}
