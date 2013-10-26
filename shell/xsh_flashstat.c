/**
 * @file     xsh_flashstat.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <flash.h>
#include <stdio.h>
#include <string.h>

#ifndef NFLASH
#define NFLASH 0
#endif

static void flashStat(ushort);

/**
 * @ingroup shell
 *
 * Shell command (flashstat) provides information about the flash device
 * @param nargs number of arguments in args array
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_flashstat(int nargs, char *args[])
{
    int n;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf
            ("\tDisplays information about the onboard flash device.\n");
        printf("Options:\n");
        printf("\t--help\t display this help and exit\n");
        return 0;
    }

    /* Too many arguments */
    if (nargs >= 2)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* Call flash stat on each flash device */
    for (n = 0; n < NFLASH; n++)
    {
        flashStat(n);
    }

    return 0;
}

static void flashStat(ushort dev)
{
#if NFLASH
    struct flash *flash;        /* pointer to flash entry   */
    struct flash_region r;      /* specific flash region    */
    uchar n;

    /* Initialize pointers */
    flash = &flashtab[dev];

    printf("%s:\n", (flash->device)->name);

    /* Flash device information */
    /* Device location */
    printf("\tFound CFI (Flash) device at 0x%08x\r\n", flash->base);

    /* Which command set it uses */
    printf("\tUsing ");
    if (FLASH_INTEL_SCS == flash->commands)
    {
        printf("Intel Simple ");
    }
    else if (FLASH_AMD_SCS == flash->commands)
    {
        printf("AMD Simple ");
    }
    printf("Command Set (0x%04x)\r\n", flash->commands);

    /* Size */
    printf("\tSize: %d bytes (split into %d blocks of %d bytes)\r\n",
           flash->size, flash->nlog_blocks, flash->log_size);

    /* Region count and info */
    printf("\tPhysically split into %d erase-regions\r\n",
           flash->nregions);
    for (n = 0; n < flash->nregions; n++)
    {
        r = flash->regions[n];
        printf("\tRegion %d at offset 0x%08x\r\n", n, r.region_start);
        printf("\t\t%d blocks of %d bytes, totaling %d bytes\r\n",
               r.nblocks, r.block_size, r.region_size);
    }
#endif
    return;
}
