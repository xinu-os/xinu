/**
 * @file     physical.c
 * A function from this file calls the proper function of the specific 
 * command set if it is supported.
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <flash.h>

/**
 * Physically read from the flash device. Passed in block must contain the
 * start_pos for the reading and the size of the block located at
 * start_pos.
 * @param *flash flash device used for data.
 * @param *block flash_block to store data in.
 * @return number of bytes read on success, SYSERR on failure.
 */
devcall physicalRead(struct flash *flash, struct flash_block *block)
{
    switch (flash->commands)
    {
    case FLASH_INTEL_SCS:
        return physicalIntelRead(flash, block);
        break;
    case FLASH_AMD_SCS:
        return physicalAMDRead(flash, block);
        break;
    default:
#ifdef DETAIL
        kprintf("Unsupported flash chipset (%d) for physical read\r\n",
                flash->commands);
#endif
        return SYSERR;
    }
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
devcall physicalWrite(struct flash *flash, struct flash_block *block)
{
    switch (flash->commands)
    {
    case FLASH_INTEL_SCS:
        return physicalIntelWrite(flash, block);
        break;
    case FLASH_AMD_SCS:
    default:
#ifdef DETAIL
        kprintf("Unsupported flash chipset (%d) for physical write\r\n",
                flash->commands);
#endif
        return SYSERR;
    }
}

/**
 * Erase an erase-block from Flash memory. Flash memory can only change
 * a binary 1 to a binary 0, not the other way around. In order to write
 * new data to the device we must tell flash to change all 0s to 1s.
 * @param *flash flash device to perform operations on
 * @param *block block to erase
 * @return OK on success, SYSERR on failure
 */
devcall physicalErase(struct flash *flash, struct flash_block *block)
{
    switch (flash->commands)
    {
    case FLASH_INTEL_SCS:
        return physicalIntelErase(flash, block);
        break;
    case FLASH_AMD_SCS:
    default:
#ifdef DETAIL
        kprintf("Unsupported flash chipset (%d) for physical erase\r\n",
                flash->commands);
#endif
        return SYSERR;
    }
}

/**
 * @param *flash flash device to use
 * @param *block block to perform operations on
 * @param func   see include/flash.h for functions
 * @param arg    argument (if needed)
 * @return status of function
 */
devcall physicalControl(struct flash *flash, struct flash_block *block,
                        uchar func, ulong arg)
{
    switch (flash->commands)
    {
    case FLASH_INTEL_SCS:
        return physicalIntelControl(flash, block, func, arg);
        break;
    case FLASH_AMD_SCS:
    default:
#ifdef DETAIL
        kprintf("Unsupported flash chipset (%d) for physical control\r\n",
                flash->commands);
#endif
        return SYSERR;
    }
}
