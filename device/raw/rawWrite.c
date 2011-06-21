/**
 * @file     rawWrite.c
 * @provides rawWrite
 *
 * $Id: rawWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <raw.h>

/**
 * Write a packet to a raw socket.
 * @param devptr pointer to RAW device
 * @param buf buffer to write
 * @param len size of the buffer 
 * @return number of octets written, SYSERR if error occurred 
 */
devcall rawWrite(device *devptr, void *buf, uint len)
{
    struct raw *rawptr;

    rawptr = &rawtab[devptr->minor];
    return rawSend(rawptr, buf, len);
}
