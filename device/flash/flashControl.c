/**
 * @file flashControl.c
 * Provides various control functions for Flash memory.
 *
 * $Id: flashControl.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <device.h>
#include <flash.h>

/**
 * Control parameters to a FLASH device. Currently supports FLASH_MODE
 * (which returns the mode of the device) and FLASH_STATUS (which will
 * return the value in the status register).
 * @param *devptr Pointer to flash device structure
 * @param func  function to use
 * @param arg1  first argument if needed in function
 * @param arg2  second argument if needed in function
 * @return dependent on function called
 */
devcall flashControl(device *devptr, ushort func, long arg1, long arg2)
{
    struct flash *flash;
    struct flash_block *e_block;
    struct flash_block l_block;
    ulong position;
    uchar n;
    ulong v;


    flash = &flashtab[devptr->minor];
    position = flash->base | arg1;

    switch (func)
    {

        /* get the size of a block, according to flash memory */
    case FLASH_BLOCK_SIZE:
        return flash->log_size;

        /* get the number of logical blocks in flash memory */
    case FLASH_N_BLOCKS:
        v = flash->nlog_blocks;
        v -= (CFE_MAX_BLOCK - CFE_MIN_BLOCK);
        v -= (KERNEL_MAX_BLOCK - KERNEL_MIN_BLOCK);
        v -= (NVRAM_MAX_BLOCK - NVRAM_MIN_BLOCK);
        return v;

        /* Force synchronization of erase-blocks in RAM and on Flash */
    case FLASH_SYNC:
        switch (arg1)
        {
        case FLASH_BLOCK:
            return physicalWrite(flash, (struct flash_block *)arg2);

        case FLASH_LOGBLOCK:
            /* check if logical block is in memory */
            arg2 += (KERNEL_MAX_BLOCK + 1);
            l_block = logicalMap(flash, (ulong)arg2);
            if (FLASH_BLOCK_DIRTY == l_block.state)
            {
                return SYSERR;
            }
            for (n = 0; n < MAX_LIVE_BLOCKS; n++)
            {
                v = (flash->curr_block + n) % MAX_LIVE_BLOCKS;
                if (l_block.start_pos == flash->erase_blocks[v].start_pos)
                {
                    /* it exists, try to write it */
                    return physicalWrite(flash,
                                         &(flash->erase_blocks[v]));
                }
            }
            /* does not exist, so we didn't write it */
            return SYSERR;

        default:
            v = OK;
            for (n = 0; n < MAX_LIVE_BLOCKS; n++)
            {
                e_block = &(flash->erase_blocks[n]);
                v |= physicalWrite(flash, e_block);
            }
            return (OK == v) ? OK : SYSERR;
        }
        break;

        /* Attempt to suspend flash from writing */
//      case FLASH_SUSPEND:

        /* Allow flash to resume writing */
//      case FLASH_RESUME:
//              break;
    }

    return SYSERR;
}
