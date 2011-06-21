/**
 * @file     flashRead.c
 * @provides flashRead
 * A simple, high-level interface to read from flash.
 *
 * $Id: flashRead.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */
#include <kernel.h>
#include <device.h>
#include <flash.h>

extern devcall logicalRead(struct flash *, uchar *, ulong);

/**
 * Provides a simple way to read in the contents of a block
 * @param *devptr   pointer to the flash device
 * @param *buffer buffer to store contents into (should be flash->log_size)
 * @param block   block number to read in
 * @return OK on success, SYSERR on failure
 */
devcall flashRead(device *devptr, uchar *buffer, ulong block)
{
    struct flash *flash;
    devcall count;

    flash = &flashtab[devptr->minor];

    /* make sure the block exists on the device */
    if (!(0 <= block && block < flash->nlog_blocks))
    {
        return SYSERR;
    }

    /* get a lock on the device, so we can read */
    wait(flash->lock);

    /* set the device to read mode */
    count = logicalRead(flash, buffer, block);

    signal(flash->lock);

    return count;
}
