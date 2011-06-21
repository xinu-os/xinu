/**
 * @file     telnetWrite.c
 * @provides telnetWrite.
 *
 * $Id: telnetWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <device.h>
#include <telnet.h>
#include <thread.h>
#include <stdio.h>

/**
 * Write a buffer to a telnet client
 * @param devptr TELNET device table entry
 * @param buf buffer of characters to output
 * @param len size of the buffer
 * @return count of characters output
 */
devcall telnetWrite(device *devptr, void *buf, uint len)
{
    struct telnet *tntptr;
    device *phw;
    uchar ch = 0;
    uint count = 0;
    uchar *buffer = buf;

    /* Setup and error check pointers to structures */
    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    wait(tntptr->osem);

    /* propery format and write all characters to buffer */
    while (count < len)
    {
        ch = buffer[count++];

        /* write buffer to underlying device if 2 more chars can't fit */
        if (tntptr->ostart >= TELNET_OBLEN - 1)
        {
            if (SYSERR == telnetFlush(devptr))
                return SYSERR;
        }

        switch (ch)
        {
            /* append CRLF to buffer, write buffer */
        case '\n':
            tntptr->out[tntptr->ostart++] = '\r';
            tntptr->out[tntptr->ostart++] = '\n';

            if (SYSERR == telnetFlush(devptr))
            {
                return SYSERR;
            }
            break;
            /* Escape IAC character */
        case TELNET_IAC:
            tntptr->out[tntptr->ostart++] = ch;
            tntptr->out[tntptr->ostart++] = ch;
            break;
            /* write normally */
        default:
            tntptr->out[tntptr->ostart++] = ch;
            break;
        }
    }

    signal(tntptr->osem);

    return count;
}
