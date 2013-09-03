/**
 * @file     physical.c
 * Functions in this file communicate directly with the hardware to achieve
 * their goals. This file acts like the BIOS in older computers (by
 * providing calls to to the physical work to the O/S).
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <flash.h>
#include <interrupt.h>
#include <memory.h>
#include <string.h>

/**
 * Physically read from the flash device. Passed in block must contain the
 * start_pos for the reading and the size of the block located at
 * start_pos.
 * @param *flash flash device used for data.
 * @param *block flash_block to store data in.
 * @return number of bytes read on success, SYSERR on failure.
 */
// This has been unchanged from the Intel Read and seems to work fine so far
devcall physicalAMDRead(struct flash *flash, struct flash_block *block)
{
    irqmask ps;
    ulong position;
    ulong size;
    uchar *buffer;
    ulong count;

    if (NULL == flash)
    {
        return SYSERR;
    }

    position = flash->base | block->start_pos;
    size = block->size;

    buffer = (uchar *)memget(size);

    if (SYSERR == (int)buffer)
    {
        return SYSERR;
    }

    ps = disable();

    for (count = 0; count < size; count++)
    {
        buffer[count] = AMD_GET_8(position + count);
    }

    restore(ps);

    if (FLASH_BLOCK_FREE == block->state)
    {
        block->state = FLASH_BLOCK_CLEAN;
    }

    block->buffer = buffer;

    flash->erase_blocks[flash->curr_block] = *block;
    flash->curr_block = (flash->curr_block + 1) % MAX_LIVE_BLOCKS;

    return count;
}
