/**
 * @file     physical.c
 *
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
devcall physicalIntelRead(struct flash *flash, struct flash_block *block)
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
        buffer[count] = INTEL_GET_8(position + count);
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

/**
 * Physically write to the flash device. This will write a stored block to
 * memory if the state if FLASH_BLOCK_DIRTY (data within the block has
 * changed). If it is not in the FLASH_BLOCK_DIRTY state that means the
 * block is either FREE (nothing is stored) or CLEAN (hasn't changed since
 * it was read).
 * @param *flash flash device used for data.
 * @param *block flash_block to store data in.
 * @return number of bytes written on success, SYSERR on failure.
 */
devcall physicalIntelWrite(struct flash *flash, struct flash_block *block)
{
    irqmask ps;
    ulong count;
    ushort data, word;
    uchar *buffer;
    ulong position;
    bool wasLocked = FALSE;

    if (NULL == flash)
    {
        return SYSERR;
    }
    if (FLASH_BLOCK_FREE == block->state)
    {
        return SYSERR;
    }

    /* not in CFE-land */
    if (!(block->start_pos > ((CFE_MAX_BLOCK + 1) * FLASH_BLK_SIZE) - 1))
    {
        return SYSERR;
    }

    /* If it is a clean block, just say we've written it */
    if (FLASH_BLOCK_CLEAN == block->state)
    {
        return block->size;
    }

    /* check if block is locked */
    if (FALSE != physicalControl(flash, block, FLASH_LOCKED, NULL))
    {
        wasLocked = TRUE;
        if (OK != physicalControl(flash, block, FLASH_UNLOCK, NULL))
        {
            return SYSERR;
        }
    }

    /* change erase-block to all 0xff (erase erase-block) */
    physicalErase(flash, block);

    buffer = (uchar *)block->buffer;
    position = flash->base | block->start_pos;

    ps = disable();

    /* write data */
    for (count = 0; count < block->size; count += 2)
    {
        /* clear status register */
        physicalControl(flash, block, FLASH_MODE, INTEL_CLR_SR);

        /* prepare to write flash */
        physicalControl(flash, block, FLASH_MODE, INTEL_PROGRAM);

        /* convert 2-bytes to a word to write */
        word = ((ushort)buffer[count + 1]) << 8;
        word = word | (ushort)buffer[count + 0];

        /* write data */
        INTEL_PUT_16(position + count, word);

        /* wait until state machine status is ready (TRUE) */
        physicalControl(flash, block, FLASH_MODE, INTEL_READ_SR);
        do
        {
            data = INTEL_GET_8(position + count);
        }
        while ((data >> INTEL_SR_WSMS) != TRUE);
    }

    physicalControl(flash, block, FLASH_MODE, INTEL_READ);

    /* if it was locked, re-lock it */
    if (wasLocked)
    {
        if (physicalControl(flash, block, FLASH_LOCK, NULL) != OK)
        {
            restore(ps);
            return SYSERR;
        }
    }

    /* ensure that the device is in read mode upon exiting function */
    physicalControl(flash, block, FLASH_MODE, INTEL_READ);

    block->state = FLASH_BLOCK_CLEAN;

    restore(ps);
    return count;
}

/**
 * Erase an erase-block from Flash memory. Flash memory can only change
 * a binary 1 to a binary 0, not the other way around. In order to write
 * new data to the device we must tell flash to change all 0s to 1s.
 * @param *flash flash device to perform operations on
 * @param *block block to erase
 * @return OK on success, SYSERR on failure
 */
devcall physicalIntelErase(struct flash *flash, struct flash_block *block)
{
    ushort data;
    irqmask ps;

    ps = disable();
    /* write erase to block to be erased */
    physicalControl(flash, block, FLASH_MODE, INTEL_ERASE);

    /* confirm block to be erased */
    physicalControl(flash, block, FLASH_MODE, INTEL_CONFIRM);

    /* confirm erase complete */
    do
    {
        data = physicalControl(flash, block, FLASH_STATUS, NULL);
        /* TODO: some sort of timeout? */
    }
    while ((data >> INTEL_SR_WSMS) != TRUE);

    physicalControl(flash, block, FLASH_MODE, INTEL_READ);

    restore(ps);

    return OK;
}

/**
 * @param *flash flash device to use
 * @param *block block to perform operations on
 * @param func   see include/flash.h for functions
 * @param arg    argument (if needed)
 * @return status of function
 */
devcall physicalIntelControl(struct flash *flash,
                             struct flash_block *block, uchar func,
                             ulong arg)
{
    ushort data;

    ulong position = flash->base | block->start_pos;

    switch (func)
    {
        /* Change the mode which Flash memory is currently in.  */
    case FLASH_MODE:
        if (flash->mode == arg)
        {
            return OK;
        }
        INTEL_PUT_8(position, arg);
        flash->mode = arg;
        return OK;

        /* Return the contents of the status register */
    case FLASH_STATUS:
        INTEL_PUT_8(position, INTEL_READ_SR);
        data = INTEL_GET_8(position);
        INTEL_PUT_8(position, INTEL_READ);
        return data;

        /* TRUE if the device is locked, FLASE if it is unlocked */
    case FLASH_LOCKED:
        physicalControl(flash, block, FLASH_MODE, INTEL_READ_ID);
        data = INTEL_GET_16(position + 0x04);
        physicalControl(flash, block, FLASH_MODE, INTEL_READ);
        return (data & INTEL_S_LOCK) ? TRUE : FALSE;

        /* Move the erase-block to an unlocked state */
    case FLASH_UNLOCK:
        INTEL_PUT_8(position, INTEL_BLOCKMOD);
        INTEL_PUT_8(position, INTEL_UNLOCK);
        physicalControl(flash, block, FLASH_MODE, INTEL_READ);
        return physicalControl(flash, block, FLASH_LOCKED,
                               NULL) ? SYSERR : OK;

        /* Move the erase-block to an locked state */
    case FLASH_LOCK:
        INTEL_PUT_8(position, INTEL_BLOCKMOD);
        INTEL_PUT_8(position, INTEL_LOCK);
        physicalControl(flash, block, FLASH_MODE, INTEL_READ);
        return physicalControl(flash, block, FLASH_LOCKED,
                               NULL) ? OK : SYSERR;
    }

    return SYSERR;
}
