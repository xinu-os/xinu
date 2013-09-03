/**
 * @file netChksum.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>

/**
 * @ingroup network
 */
ushort netChksum(void *data, uint len)
{
    uint sum;
    ushort *ptr;

    /* Add data to sum */
    ptr = (ushort *)data;
    sum = 0;
    while (len > 1)
    {
        sum += *ptr;
        ptr++;
        len -= 2;
    }

    /* Add left-over byte, if any */
    if (len > 0)
    {
        sum += net2hs(*((uchar *)ptr) << 8);
    }

    /* Fold 32-bit sum into 16 bits */
    while (sum >> 16)
    {
        sum = (sum >> 16) + (sum & 0xFFFF);
    }

    return (~sum);
}
