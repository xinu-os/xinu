/**
 * @file backplaneInit.c
 * @provides backplaneInit.
 *
 * $Id: backplaneInit.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <backplane.h>

struct chipcregs *sbCore;

/**
 * Initialize Silicon Backplane bus interface.
 * @param devptr pointer to an sb-bus device
 */
devcall backplaneInit(device *devptr)
{
#if SB_BUS
    int numcores = 0;
    ushort vendor = 0;
    ushort coreid = 0;
    uchar *scanptr = NULL;
    char *vendStr = NULL;
    char *devcStr = NULL;

    sbCore = (struct chipcregs *)SB_ENUM_BASE;

    numcores = (sbCore->chipid & CID_CC_MASK) >> CID_CC_SHIFT;

#ifdef DETAIL
    kprintf
        ("Silicon Backplane Chip ID %04X, rev %d.  Number of cores = %d\r\n",
         sbCore->chipid & CID_ID_MASK,
         (sbCore->chipid & CID_REV_MASK) >> CID_REV_SHIFT, numcores);
#endif                          /* DETAIL */

    for (scanptr = (uchar *)SB_ENUM_BASE;
         (long)scanptr <
         SB_ENUM_BASE + numcores * SB_CORE_SIZE; scanptr += SB_CORE_SIZE)
    {
        vendor =
            (*(ulong *)
             (scanptr + SBCONFIGOFF +
              SBIDHIGH) & SBIDH_VC_MASK) >> SBIDH_VC_SHIFT;
        coreid =
            (*(ulong *)
             (scanptr + SBCONFIGOFF +
              SBIDHIGH) & SBIDH_CC_MASK) >> SBIDH_CC_SHIFT;

        if (SB_VEND_BCM == vendor)
        {
            vendStr = "Broadcom";
        }
        else
        {
            vendStr = "Unknown Vendor";
        }
        switch (coreid)
        {
        case SB_CC:
            devcStr = "Chip Common";
            break;
        case SB_ILINE20:
            devcStr = "Iline20";
            break;
        case SB_SDRAM:
            devcStr = "SDRAM";
            break;
        case SB_PCI:
            devcStr = "PCI";
            break;
        case SB_MIPS:
            devcStr = "MIPS";
            break;
        case SB_ENET:
            devcStr = "Ethernet MAC";
            break;
        case SB_CODEC:
            devcStr = "v90 Codec";
            break;
        case SB_USB:
            devcStr = "USB 1.1 Host/Device";
            break;
        case SB_ADSL:
            devcStr = "ADSL";
            break;
        case SB_ILINE100:
            devcStr = "Iline 100";
            break;
        case SB_IPSEC:
            devcStr = "IPSec";
            break;
        case SB_PCMCIA:
            devcStr = "PCMCIA";
            break;
        case SB_SOCRAM:
            devcStr = "Internal Memory";
            break;
        case SB_MEMC:
            devcStr = "Memc SDRAM";
            break;
        case SB_EXTIF:
            devcStr = "External Interface";
            break;
        case SB_D11:
            devcStr = "802.11 MAC";
            break;
        case SB_MIPS33:
            devcStr = "MIPS 3302";
            break;
        case SB_USB11H:
            devcStr = "USB 1.1 Host";
            break;
        case SB_USB11D:
            devcStr = "USB 1.1 Device";
            break;
        case SB_USB20H:
            devcStr = "USB 2.0 Host";
            break;
        case SB_USB20D:
            devcStr = "USB 2.0 Device";
            break;
        case SB_SDIOH:
            devcStr = "SDIO Host";
            break;
        case SB_ROBO:
            devcStr = "RoboSwitch";
            break;
        case SB_ATA100:
            devcStr = "Parallel ATA";
            break;
        case SB_SATAXOR:
            devcStr = "Serial ATA & XOR DMA";
            break;
        case SB_GIGETH:
            devcStr = "Gigabit Ethernet";
            break;

        default:
            devcStr = "Unknown";
        }

#ifdef DETAIL
        kprintf(" Found %s %20s Core (%03X) at 0x%08X\r\n",
                vendStr, devcStr, coreid, scanptr);
#endif                          /* DETAIL */
    }

#ifdef DETAIL
    kprintf("\r\n");
#endif                          /* DETAIL */

#endif                          /* SB_BUS */
    return OK;
}
