/**
 * @file     flashClose.c
 *
 */
#include <flash.h>
#include <stdarg.h>

/**
 * This will close the flash device, forcing the synchronization of 
 * disk blocks stored in memory and the flash device.
 * @param *devptr device to open
 * @return OK on success, SYSERR on failure
 */
devcall flashClose(device *devptr)
{
    struct flash *flash;
    struct flash_block block;
    uchar n;

    flash = &flashtab[devptr->minor];

    for (n = 0; n < MAX_LIVE_BLOCKS; n++)
    {
        /* fetch struct for zeroing */
        block = flash->erase_blocks[n];

        if (FLASH_BLOCK_DIRTY == block.state)
        {
            /* the block is dirty, write to flash */
            return SYSERR;
        }

        /* reset block to safe values */
        block.start_pos = 0;
        block.state = FLASH_BLOCK_FREE;
        block.buffer = NULL;
        block.size = 0;

        /* write block to flash struct */
        flash->erase_blocks[n] = block;
    }
    signal(flash->lock);
    return OK;
}
