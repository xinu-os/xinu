/**
 * @file     flashWrite.c
 * A simple, high-level interface to write to flash.
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */
#include <kernel.h>
#include <device.h>
#include <flash.h>

/**
 * Provides a simple way to write the contents of a block to flash.
 * @param *devptr   pointer to the flash device
 * @param *buffer buffer holding content to write (should be flash->log_size)
 * @param block   block number to write to
 * @return OK on success, SYSERR on failure
 */
devcall flashWrite(device *devptr, uchar *buffer, ulong block)
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
    count = logicalWrite(flash, buffer, block);

    signal(flash->lock);

    return count;
}
