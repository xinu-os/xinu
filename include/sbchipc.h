/*
 * SiliconBackplane Chipcommon core hardware definitions.
 *
 * The chipcommon core provides chip identification, SB control,
 * jtag, 0/1/2 uarts, clock frequency control, a watchdog interrupt timer,
 * gpio interface, extbus, and support for serial and parallel flashes.
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef _SBCHIPC_H_
#define _SBCHIPC_H_

#include <stddef.h>

struct chipcregs
{
    volatile ulong chipid;      /* 0x0 */
    volatile ulong capabilities;
    volatile ulong corecontrol; /* corerev >= 1 */
    volatile ulong pad0[5];

    /* Interrupt control */
    volatile ulong intstatus;   /* 0x20 */
    volatile ulong intmask;
    volatile ulong pad1[6];

    /* serial flash interface registers */
    volatile ulong flashcontrol;        /* 0x40 */
    volatile ulong flashaddress;
    volatile ulong flashdata;
    volatile ulong pad2[1];

    /* Silicon backplane configuration broadcast control */
    volatile ulong broadcastaddress;
    volatile ulong broadcastdata;
    volatile ulong pad3[2];

    /* gpio - cleared only by power-on-reset */
    volatile ulong gpioin;      /* 0x60 */
    volatile ulong gpioout;
    volatile ulong gpioouten;
    volatile ulong gpiocontrol;
    volatile ulong gpiointpolarity;
    volatile ulong gpiointmask;
    volatile ulong pad4[2];

    /* Watchdog timer */
    volatile ulong watchdog;    /* 0x80 */
    volatile ulong pad5[3];

    /* clock control */
    volatile ulong clockcontrol_n;      /* 0x90 */
    volatile ulong clockcontrol_sb;     /* aka m0 */
    volatile ulong clockcontrol_pci;    /* aka m1 */
    volatile ulong clockcontrol_m2;     /* mii/uart/mipsref */
    volatile ulong clockcontrol_mips;   /* aka m3 */
    volatile ulong uart_clkdiv; /* corerev >= 3 */
    volatile ulong pad6[2];

    /* pll delay registers (corerev >= 4) */
    volatile ulong pll_on_delay;        /* 0xb0 */
    volatile ulong fref_sel_delay;
    volatile ulong slow_clk_ctl;
    volatile ulong pad7[17];

    /* ExtBus control registers (corerev >= 3) */
    volatile ulong cs01config;  /* 0x100 */
    volatile ulong cs01memwaitcnt;
    volatile ulong cs01attrwaitcnt;
    volatile ulong cs01iowaitcnt;
    volatile ulong cs23config;
    volatile ulong cs23memwaitcnt;
    volatile ulong cs23attrwaitcnt;
    volatile ulong cs23iowaitcnt;
    volatile ulong cs4config;
    volatile ulong cs4waitcnt;
    volatile ulong parallelflashconfig;
    volatile ulong parallelflashwaitcnt;
    volatile ulong pad8[116];

    /* uarts */
    volatile uchar uart0data;   /* 0x300 */
    volatile uchar uart0imr;
    volatile uchar uart0fcr;
    volatile uchar uart0lcr;
    volatile uchar uart0mcr;
    volatile uchar uart0lsr;
    volatile uchar uart0msr;
    volatile uchar uart0scratch;
    volatile uchar pad9[248];   /* corerev >= 1 */

    volatile uchar uart1data;   /* 0x400 */
    volatile uchar uart1imr;
    volatile uchar uart1fcr;
    volatile uchar uart1lcr;
    volatile uchar uart1mcr;
    volatile uchar uart1lsr;
    volatile uchar uart1msr;
    volatile uchar uart1scratch;
};

/* chipid */
#define CID_ID_MASK   0x0000ffff        /* Chip Id mask */
#define CID_REV_MASK  0x000f0000        /* Chip Revision mask */
#define CID_REV_SHIFT 16        /* Chip Revision shift */
#define CID_PKG_MASK  0x00f00000        /* Package Option mask */
#define CID_PKG_SHIFT 20        /* Package Option shift */
#define CID_CC_MASK   0x0f000000        /* CoreCount (corerev >= 4) */
#define CID_CC_SHIFT  24

/* capabilities */
#define CAP_UARTS_MASK 0x00000003       /* Number of uarts */
#define CAP_MIPSEB     0x00000004       /* MIPS is in big-endian mode */
#define CAP_UCLKSEL    0x00000018       /* UARTs clock select */
#define CAP_UINTCLK    0x00000008       /* UARTs driven by internal clock */
#define CAP_UARTGPIO   0x00000020       /* UARTs own Gpio's 15:12 */
#define CAP_EXTBUS     0x00000040       /* External bus present */
#define CAP_FLASH_MASK 0x00000700       /* Type of flash */
#define CAP_PLL_MASK   0x00038000       /* Type of PLL */
#define CAP_PWR_CTL    0x00040000       /* Power control */

/* PLL type */
#define PLL_NONE  0x00000000
#define PLL_TYPE1 0x00010000    /* 48Mhz base, 3 dividers */
#define PLL_TYPE2 0x00020000    /* 48Mhz, 4 dividers */
#define PLL_TYPE3 0x00030000    /* 25Mhz, 2 dividers */
#define PLL_TYPE4 0x00008000    /* 48Mhz, 4 dividers */

/* corecontrol */
#define CC_UARTCLKO 0x00000001  /* Drive UART with internal clock */
#define CC_SE       0x00000002  /* sync clk out enable (corerev >= 3) */

/* intstatus/intmask */
#define CI_EI   0x00000002      /* ro: ext intr pin (corerev >= 3) */

/* slow_clk_ctl */
#define SCC_SS_MASK 0x00000007  /* slow clock source mask */
#define SCC_SS_LPO  0x00000000  /* source of slow clock is LPO */
#define SCC_SS_XTAL 0x00000001  /* source of slow clock is crystal */
#define SCC_SS_PCI  0x00000002  /* source of slow clock is PCI */
#define SCC_LF      0x00000200  /* LPOFreqSel, 1: 160Khz, 0: 32KHz */
#define SCC_LP      0x00000400  /* LPOPowerDown, 1/0: disabled/enabled */
#define SCC_FS      0x00000800  /* ForceSlowClk, 1/0: slow clock/logic */
/* IgnorePllOffReq, 1/0: logic ignores/honors PLL clock disable */
#define SCC_IP      0x00001000
/* XtalControlEn, 1/0: power logic does/doesn't disable crystal */
#define SCC_XC      0x00002000
/* XtalPU (RO), 1/0: crystal running/disabled */
#define SCC_XP      0x00004000
/* ClockDivider mask, SlowClk = 1/(4+divisor) * crystal/PCI clock */
#define SCC_CD_MASK 0xffff0000
#define SCC_CD_SHF  16          /* CLockDivider shift */

/* clockcontrol_n */
#define CN_N1_MASK  0x3f        /* n1 control */
#define CN_N2_MASK  0x3f00      /* n2 control */
#define CN_N2_SHIFT 8

/* clockcontrol_sb/pci/uart */
#define CC_M1_MASK  0x3f        /* m1 control */
#define CC_M2_MASK  0x3f00      /* m2 control */
#define CC_M2_SHIFT 8
#define CC_M3_MASK  0x3f0000    /* m3 control */
#define CC_M3_SHIFT 16
#define CC_MC_MASK  0x1f000000  /* mux control */
#define CC_MC_SHIFT 24

/* N3M Clock control values for 125Mhz */
#define CC_125_N   0x0802       /* Default values for bcm4310 */
#define CC_125_M   0x04020009
#define CC_125_M25 0x11090009
#define CC_125_M33 0x11090005

/* N3M Clock control magic field values */
#define CC_F6_2   0x02
                                                                                /* A factor of 2 in */
#define CC_F6_3   0x03
                                                                                /* 6-bit fields like */
#define CC_F6_4   0x05
                                                                                /* N1, M1 or M3 */
#define CC_F6_5   0x09
#define CC_F6_6   0x11
#define CC_F6_7   0x21

#define CC_F5_BIAS  5           /* 5-bit fields get this added */

#define CC_MC_BYPASS 0x08
#define CC_MC_M1     0x04
#define CC_MC_M1M2   0x02
#define CC_MC_M1M2M3 0x01
#define CC_MC_M1M3   0x11

/* Type 2 Clock control magic field values */
#define CC_T2_BIAS   2          /* n1, n2, m1 & m3 bias */
#define CC_T2M2_BIAS 3          /* m2 bias */

#define CC_T2MC_M1BYP 1
#define CC_T2MC_M2BYP 2
#define CC_T2MC_M3BYP 4

/* Common clock base */
#define CC_CLOCK_BASE 24000000  /* Half the clock freq */

/* Flash types in the chipcommon capabilities register */
#define FLASH_NONE 0x000        /* No flash */
#define SFLASH_ST  0x100        /* ST serial flash */
#define SFLASH_AT  0x200        /* Atmel serial flash */
#define PFLASH     0x700
                                                                                /* Parallel flash */

/* Bits in the config registers */
#define CC_CFG_EN        0x0001 /* Enable */
#define CC_CFG_EM_MASK   0x000e /* Extif Mode */
#define CC_CFG_EM_ASYNC  0x0002 /*   Async/Parallel flash */
#define CC_CFG_EM_SYNC   0x0004 /*   Synchronous */
#define CC_CFG_EM_PCMCIA 0x0008 /*   PCMCIA */
#define CC_CFG_EM_IDE    0x000a /*   IDE */
#define CC_CFG_DS        0x0010 /* Data size, 0=8bit, 1=16bit */
#define CC_CFG_CD_MASK   0x0060 /* Sync: Clock divisor */
#define CC_CFG_CE        0x0080 /* Sync: Clock enable */
#define CC_CFG_SB        0x0100 /* Sync: Size/Bytestrobe */

/* Start/busy bit in flashcontrol */
#define SFLASH_START  0x80000000
#define SFLASH_BUSY  SFLASH_START

/* flashcontrol opcodes for ST flashes */
#define SFLASH_ST_WREN  0x0006  /* Write Enable */
#define SFLASH_ST_WRDIS 0x0004  /* Write Disable */
#define SFLASH_ST_RDSR  0x0105  /* Read Status Register */
#define SFLASH_ST_WRSR  0x0101  /* Write Status Register */
#define SFLASH_ST_READ  0x0303  /* Read Data Bytes */
#define SFLASH_ST_PP    0x0302  /* Page Program */
#define SFLASH_ST_SE    0x02d8  /* Sector Erase */
#define SFLASH_ST_BE    0x00c7  /* Bulk Erase */
#define SFLASH_ST_DP    0x00b9  /* Deep Power-down */
#define SFLASH_ST_RES   0x03ab  /* Read Electronic Signature */

/* Status register bits for ST flashes */
#define SFLASH_ST_WIP      0x01 /* Write In Progress */
#define SFLASH_ST_WEL      0x02 /* Write Enable Latch */
#define SFLASH_ST_BP_MASK  0x1c /* Block Protect */
#define SFLASH_ST_BP_SHIFT 2
#define SFLASH_ST_SRWD     0x80 /* Status Register Write Disable */

/* flashcontrol opcodes for Atmel flashes */
#define SFLASH_AT_READ                0x07e8
#define SFLASH_AT_PAGE_READ           0x07d2
#define SFLASH_AT_BUF1_READ
#define SFLASH_AT_BUF2_READ
#define SFLASH_AT_STATUS              0x01d7
#define SFLASH_AT_BUF1_WRITE          0x0384
#define SFLASH_AT_BUF2_WRITE          0x0387
#define SFLASH_AT_BUF1_ERASE_PROGRAM  0x0283
#define SFLASH_AT_BUF2_ERASE_PROGRAM  0x0286
#define SFLASH_AT_BUF1_PROGRAM        0x0288
#define SFLASH_AT_BUF2_PROGRAM        0x0289
#define SFLASH_AT_PAGE_ERASE          0x0281
#define SFLASH_AT_BLOCK_ERASE         0x0250
#define SFLASH_AT_BUF1_WRITE_ERASE_PROGRAM 0x0382
#define SFLASH_AT_BUF2_WRITE_ERASE_PROGRAM 0x0385
#define SFLASH_AT_BUF1_LOAD           0x0253
#define SFLASH_AT_BUF2_LOAD           0x0255
#define SFLASH_AT_BUF1_COMPARE        0x0260
#define SFLASH_AT_BUF2_COMPARE        0x0261
#define SFLASH_AT_BUF1_REPROGRAM      0x0258
#define SFLASH_AT_BUF2_REPROGRAM      0x0259

/* Status register bits for Atmel flashes */
#define SFLASH_AT_READY    0x80
#define SFLASH_AT_MISMATCH 0x40
#define SFLASH_AT_ID_MASK  0x38
#define SFLASH_AT_ID_SHIFT 3

#endif                          /* _SBCHIPC_H_ */
