/**
 * @file     flashOpen.c
 * @provides flashOpen
 * Open a flash device for reading and writing.
 *
 * $Id: flashOpen.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <flash.h>

extern devcall superblockOpen(ushort);

/**
 * This will open the flash device for read/write access.
 * @param *devptr device to open
 * @param ap list or arguments
 * @return OK on success, SYSERR on failure
 */
devcall flashOpen(device *devptr, va_list ap)
{
    struct flash *flash;
    struct flash_block block;
    uchar n;

    flash = &flashtab[devptr->minor];
    wait(flash->lock);
    for (n = 0; n < MAX_LIVE_BLOCKS; n++)
    {
        /* fetch struct for zeroing */
        block = flash->erase_blocks[n];

        /* clear struct to known values */
        block.start_pos = 0;
        block.state = FLASH_BLOCK_FREE;
        block.buffer = NULL;
        block.size = 0;

        /* write block to flash struct */
        flash->erase_blocks[n] = block;
    }

    //superblockOpen(dptr->num);

    return OK;
}
