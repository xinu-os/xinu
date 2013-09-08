/**
 * @file     flashInit.c
 * Flash initialization.  This should be called during general device
 * initialization and will detect a CFI flash device.
 *
 */
#include <kernel.h>
#include <stdlib.h>
#include <flash.h>
#include "platformflash.h"

#ifndef NFLASH
#define NFLASH 0
#endif

struct flash flashtab[NFLASH];
struct flash_block bad_block = { 0, 0, FLASH_BLOCK_DIRTY, NULL };

/**
 * Initialized the flash device located beginning at the value of
 * this devices CSR.
 * @param *devptr pointer to device entry
 * @return OK on success, SYSERR on failure
 */
devcall flashInit(device *devptr)
{
    struct flash *flash;
    struct flash_region r;
    uchar n;
    ulong position;

    flash = &flashtab[devptr->minor];
    bzero((void *)flash, sizeof(struct flash));

    flash->device = devptr;
    flash->base = (ulong)devptr->csr;
    flash->lock = semcreate(1);

    /* Change to CFI query mode */
    CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_MODE);

    /* XINU only supports valid CFI devices */
    if (!(CFI_GET_8(flash->base, CFI_SIGNATURE + 0) == 'Q'
          && CFI_GET_8(flash->base, CFI_SIGNATURE + 1) == 'R'
          && CFI_GET_8(flash->base, CFI_SIGNATURE + 2) == 'Y'))
    {
        /* Not a CFI flash device, it may still be flash */
        /* but XINU will not support those               */
        CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_EXIT);
        return SYSERR;
    }

    /* We know it is a valid CFI device, now get some data */
    /* what command set does it use */
    flash->commands = CFI_GET_8(flash->base, CFI_COMMAND_SET);

    /* XINU might support both Intel and AMD's Standard Command Set */
    if (!(flash->commands == FLASH_INTEL_SCS
          || flash->commands == FLASH_AMD_SCS))
    {
        CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_EXIT);
        return SYSERR;
    }

    /* total device size (2^n) */
    flash->size = 1 << CFI_GET_8(flash->base, CFI_DEV_SIZE);

    /* number of erase-block regions */
    flash->nregions = CFI_GET_8(flash->base, CFI_REGION_COUNT);

    if (flash->nregions >= MAX_REGIONS)
    {
        CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_EXIT);
        return SYSERR;
    }

    /* get information about each region */
    position = 0;
    for (n = 0; n < flash->nregions; n++)
    {
        r = flash->regions[n];

        /* find the offset in flash */
        r.region_start = position;

        /* number of erase-blocks in region */
        r.nblocks = 1 + CFI_GET_8(flash->base, CFI_REGION_TAB + n * 4 + 0)
            + (CFI_GET_8(flash->base, CFI_REGION_TAB + n * 4 + 1) << 8);

        /* size of erase-blocks within region */
        r.block_size = (CFI_GET_8(flash->base, CFI_REGION_TAB + n * 4 + 2)
                        +
                        (CFI_GET_8
                         (flash->base, CFI_REGION_TAB + n * 4 + 3) << 8));
        r.block_size *= 256;

        /* store the size of this region */
        r.region_size = r.nblocks * r.block_size;

        /* iterate for the position of the next region */
        position += r.region_size;

        /* store the updated region in struct flash */
        flash->regions[n] = r;
    }

    /* Dumb check to make sure all regions are counted */
    if (position != flash->size)
    {
        CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_EXIT);
        return SYSERR;
    }

    /* Set the size of logical blocks for this disk */
    flash->log_size = FLASH_BLK_SIZE;

    /* Determine how many logical blocks exist */
    flash->nlog_blocks = flash->size / flash->log_size;

    /* Return to read mode */
    CFI_PUT_8(flash->base, CFI_QUERY_ADDR, CFI_QUERY_EXIT);
    flash->mode = INTEL_READ;

#ifdef DETAIL
    kprintf("Found CFI (Flash) device at 0x%08x\r\n", flash->base);
    kprintf("  Using 0x%04x command set\r\n", flash->commands);
    kprintf("  Size: %d bytes (split into %d blocks of %d bytes)\r\n",
            flash->size, flash->nlog_blocks, flash->log_size);
    kprintf("  Physically split into %d erase-regions\r\n",
            flash->nregions);
    for (n = 0; n < flash->nregions; n++)
    {
        r = flash->regions[n];
        kprintf("  Region %d at offset 0x%08x\r\n", n, r.region_start);
        kprintf("    %d blocks of %d bytes, totaling %d bytes\r\n",
                r.nblocks, r.block_size, r.region_size);
    }
    kprintf("\n");
#endif

    return OK;
}
