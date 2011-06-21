/**
 * @file tcpSeqdiff.c
 * @provides tcpSeqdiff
 *
 * $Id: tcpSeqdiff.c 2134 2009-11-20 02:12:30Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <tcp.h>

/**
 * Calculates the difference between two sequence numbers.
 * @param first sequence number that occurs first
 * @param second sequence number that occurs second
 * @return difference between two sequence numbers
 */
tcpseq tcpSeqdiff(tcpseq first, tcpseq second)
{
/*	if (first <= second)
	{ return (second-first); }
	else
	{ return ((TCP_MAXSEQ - first) + second); }*/

    if ((first < (TCP_MAXSEQ / 2)) && (second > (TCP_MAXSEQ / 2)))
    {
        return ((TCP_MAXSEQ - first) + second);
    }

    if ((second < (TCP_MAXSEQ / 2)) && (first > (TCP_MAXSEQ / 2)))
    {
        return ((TCP_MAXSEQ - first) + second);
    }

    return first - second;
}
