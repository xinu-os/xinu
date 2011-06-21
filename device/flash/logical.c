/**
 * @file     logical.c
 * @provides logicalMap, logicalWrite, logicalRead
 * This provides a more proper interface between driver calls and the
 * physical layer of flash. It is possible to split the entire flash device
 * into logical blocks of n bytes and this file provides the mappings from
 * physical address to logical block.
 *
 * $Id: logical.c 2051 2009-08-27 20:55:09Z akoehler $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <flash.h>
#include <string.h>
#include <memory.h>

/**
 * With a logical block address, read in the contents of the block into the
 * buffer. This will check if the erase-block is already in memory and if
 * so simply read from the cached copy, if it is not read it into memory
 * and read the specified block. If there is no more room in memory to
 * store an erase-block it will automatically write to the oldest
 * erase-block to flash and read in the requested block.
 * @param *flash  flash device to read on.
 * @param *buffer buffer to read into.
 * @param block   block to read on device.
 * @return number of bytes read or SYSERR on failure
 */
devcall logicalRead(struct flash *flash, uchar *buffer, ulong block)
{
    struct flash_block new_block, test_block;
    uchar n, pos;

    /* real addresses start right after the kernel location */
    /* block 0 maps to block 0x400 */
    block += KERNEL_MAX_BLOCK + 1;

    new_block = logicalMap(flash, block);

    if (FLASH_BLOCK_DIRTY == new_block.state)
    {
        return SYSERR;
    }

    for (n = 0; n < MAX_LIVE_BLOCKS; n++)
    {
        pos = (flash->curr_block + n) % MAX_LIVE_BLOCKS;
        test_block = flash->erase_blocks[pos];

        if (test_block.start_pos == new_block.start_pos)
        {
            if (FLASH_BLOCK_FREE != test_block.state)
            {
                /* this is already in memory, read now! */
                new_block = test_block;
                break;
            }
        }
    }

    if (MAX_LIVE_BLOCKS <= n)
    {
        /* we checked all blocks, none match, none free */
        /* evict and read */
        physicalWrite(flash, &(flash->erase_blocks[flash->curr_block]));
        new_block.state = FLASH_BLOCK_FREE;
    }

    if (FLASH_BLOCK_FREE == new_block.state)
    {
        /* block is free, read into pos */
        physicalRead(flash, &new_block);
    }

    /* test_block now has the data we want, we just need to fetch it */

    /* turn block into a physical offset */
    block = block * flash->log_size;

    /* make block an offset into the erase-block */
    block -= new_block.start_pos;

    memcpy(buffer, new_block.buffer + block, flash->log_size);

    return flash->log_size;
}

/**
 * Translate a logical block number to a physical portion of flash and
 * write the data. This function will check if the erase-block is in memory
 * and store changes to the memory location to prevent too many writes to
 * flash. If no more room is available it will evict the oldest erase-block
 * and write it to flash.
 * @param *flash  flash device to write to
 * @param *buffer buffer holding new data
 * @param block   block to write to device
 * @return number of bytes written or SYSERR on failure
 */
devcall logicalWrite(struct flash *flash, uchar *buffer, ulong block)
{
    struct flash_block new_block, test_block;
    uchar n, pos;

    /* real addresses start right after the kernel location */
    /* block 0 maps to block 0x400 */
    block += KERNEL_MAX_BLOCK + 1;

    /* there should be a partition map, but for simplicity we do checks */
    /* Make sure block is not in CFE */
    if ((CFE_MIN_BLOCK <= block && block <= CFE_MAX_BLOCK))
    {
        return SYSERR;
    }
    /* Make sure block is not in NVRAM settings */
    if ((NVRAM_MIN_BLOCK <= block && block <= NVRAM_MAX_BLOCK))
    {
        return SYSERR;
    }
    /* Make sure block is not in kernel space */
    if ((KERNEL_MIN_BLOCK <= block && block <= KERNEL_MAX_BLOCK))
    {
        return SYSERR;
    }

    new_block = logicalMap(flash, block);

    if (FLASH_BLOCK_DIRTY == new_block.state)
    {
        return SYSERR;
    }

    for (n = 0; n < MAX_LIVE_BLOCKS; n++)
    {
        pos = (flash->curr_block + n) % MAX_LIVE_BLOCKS;
        test_block = flash->erase_blocks[pos];

        if (test_block.start_pos == new_block.start_pos)
        {
            if (FLASH_BLOCK_FREE != test_block.state)
            {
                new_block = test_block;
                break;
            }
        }
    }

    new_block.state = FLASH_BLOCK_DIRTY;

    if (MAX_LIVE_BLOCKS <= n)
    {
        /* we search all the blocks, but found nothing. */
        /* evict and load */
        physicalWrite(flash, &(flash->erase_blocks[flash->curr_block]));
        physicalRead(flash, &new_block);
    }
    else
    {
        flash->erase_blocks[pos] = new_block;
    }

    /* turn block into a physical offset */
    block = block * flash->log_size;

    /* make block an offset into the erase-block */
    block -= new_block.start_pos;

    memcpy(new_block.buffer + block, buffer, flash->log_size);

    return flash->log_size;
}

/**
 * Given a flash structure and a logical block, this method will
 * discover the physical erase-block (and region) of that block.
 * Once the physical mapping is found it will return an incomplete
 * flash_block structure with the region and erase-block filled in.
 * @param *flash flash device to discover on.
 * @param block  logical block to find.
 * @return flash_block structure with start_pos and size
 */
struct flash_block logicalMap(struct flash *flash, ulong block)
{
    struct flash_block f_block; /* flash block to store information */
    ulong e_region, e_block;    /* erase region and block           */
    struct flash_region f_region;       /* flash region block is in         */
    ulong position = 0;         /* log. block of data search is at  */

    e_region = 0;
    while (e_region < flash->nregions)
    {
        f_region = flash->regions[e_region];

        /* how many blocks does the current region have? */
        position += f_region.region_size;

        /* skipping this region is too far it must be in this block */
        if ((position / flash->log_size) > block)
        {
            break;
        }

        e_region++;
    }

    /* make sure we didn't go too far */
    if (e_region >= flash->nregions)
    {
        return bad_block;
    }

    /* back to the beginning of this region */
    position -= f_region.region_size;

    e_block = 0;
    while (e_block < f_region.nblocks)
    {
        /* how many logical blocks in this erase block? */
        position += f_region.block_size;

        /* skipping this erase-block is too far */
        if ((position / flash->log_size) > block)
        {
            break;
        }

        e_block++;
    }

    /* make sure we didn't break the loop */
    if (e_block >= f_region.nblocks)
    {
        return bad_block;
    }

    f_block.start_pos = position - f_region.block_size;
    f_block.size = f_region.block_size;

    /* not an allocated block, make sure it fails safety checks */
    f_block.state = FLASH_BLOCK_FREE;

    return f_block;
}
