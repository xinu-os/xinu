/**
 * @file switchInit.c 
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <conf.h>
#include "bcm4713.h"
#include "bcmswitch.h"

int waitOnBit(volatile uint *reg, uint mask, const int value, int tries);

struct swtstate swtregs;

int switchInit(struct bcm4713 *nicptr)
{
    int result = 0, members = 0, untag = 0, vlan = 0, port = 0;
    struct mdioregs *mdio = NULL;
    ushort deftags[SWT_PORTS];

    swtregs.mdio = NULL;
    swtregs.page = -1;

    if (NULL == nicptr)
    {
        return SYSERR;
    }
    mdio = (struct mdioregs *)&nicptr->mdioControl;
    swtregs.mdio = mdio;

    for (port = 0; port < (SWT_PORTS - NETHER); port++)
    {
        /* Initialize default VLAN */
        members |= (1 << port);
        untag |= (1 << port);
        deftags[port] = vlan;
    }
    members |= (1 << SWT_CPUPORT);      /* Always include CPU interface */
    result = ((members | (untag << SWT_PORTS))
              & (SWT_VLAN_GROUPMASK | SWT_VLAN_UNTAGMASK));

    result |= SWT_VLAN_WRITE_VALID;
    switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_WRITE, &result);
    result = SWT_VLAN_TABACC_ENABLE | SWT_VLAN_TABACC_WRITE | vlan;
    switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_TABACC, &result);

    deftags[SWT_CPUPORT] = vlan;

    for (port = (SWT_PORTS - NETHER); port < SWT_CPUPORT; port++)
    {
        /* Initialize optional LANs */
        vlan++;
        members = (1 << port) | (1 << SWT_CPUPORT);
        untag = (1 << port);
        result = ((members | (untag << SWT_PORTS))
                  & (SWT_VLAN_GROUPMASK | SWT_VLAN_UNTAGMASK));

        result |= SWT_VLAN_WRITE_VALID;
        switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_WRITE, &result);
        result = SWT_VLAN_TABACC_ENABLE | SWT_VLAN_TABACC_WRITE | vlan;
        switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_TABACC, &result);
        deftags[port] = vlan;
    }
    for (port = SWT_CPUPORT; port < SWT_VLAN_TABLEMAX; port++)
    {
        /* Clear remaining table entries */
        vlan++;
        result = SWT_VLAN_WRITE_VALID;
        switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_WRITE, &result);
        result = SWT_VLAN_TABACC_ENABLE | SWT_VLAN_TABACC_WRITE | vlan;
        switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_TABACC, &result);
    }

    for (port = 0; port < SWT_PORTS; port++)
    {
        /* Program default port tags */
        result = deftags[port];
        switchWriteReg(mdio, SWT_PAGE_VLAN,
                       SWT_REG_VLAN_TAGPORT0 + 2 * port, &result);
    }

    return OK;
}

#define MII_MAX_RETRY   100
#define PSEUDO_PHYAD    0x1E    /* MII Pseudo PHY address */

/* MII registers */
#define REG_MII_PAGE    0x10    /* MII Page register */
#define REG_MII_ADDR    0x11    /* MII Address register */
#define REG_MII_DATA0   0x18    /* MII Data register 0 */
#define REG_MII_DATA1   0x19    /* MII Data register 1 */

/* emac mdio data */
#define MD_DATA_MASK    0xffff  /* r/w data */
#define MD_TA_MASK      0x30000 /* turnaround value */
#define MD_TA_SHIFT     16
#define MD_TA_VALID     (2 << MD_TA_SHIFT)      /* valid ta */
#define MD_RA_MASK      0x7c0000        /* register address */
#define MD_RA_SHIFT     18
#define MD_PMD_MASK     0xf800000       /* physical media device */
#define MD_PMD_SHIFT    23
#define MD_OP_MASK      0x30000000      /* opcode */
#define MD_OP_SHIFT     28
#define MD_OP_WRITE     (1 << MD_OP_SHIFT)      /* write op */
#define MD_OP_READ      (2 << MD_OP_SHIFT)      /* read op */
#define MD_SB_MASK      0xc0000000      /* start bits */
#define MD_SB_SHIFT     30
#define MD_SB_START     (0x1 << MD_SB_SHIFT)    /* start of frame */

#define EI_MII          ((ulong)1 << 0) /* mii mdio interrupt */
#define EI_MIB          ((ulong)1 << 1) /* mib interrupt */
#define EI_FLOW         ((ulong)1 << 2) /* flow control interrupt */

static ushort mdioRead(struct mdioregs *mdio, ulong phyaddr, ulong reg)
{
    /* clear mii_int */
    mdio->emacIntstatus = EI_MII;

    mdio->mdioData =
        (MD_SB_START | MD_OP_READ | (phyaddr << MD_PMD_SHIFT) |
         (reg << MD_RA_SHIFT) | MD_TA_VALID);

    /* wait for it to complete */
    waitOnBit((uint *)&mdio->emacIntstatus, EI_MII, 1, 100);

    return (mdio->mdioData & MD_DATA_MASK);
}

static void mdioWrite(struct mdioregs *mdio, ulong phyaddr, ulong reg,
                      ushort v)
{
    /* clear mii_int */
    mdio->emacIntstatus = EI_MII;

    /* issue the write */
    mdio->mdioData =
        (MD_SB_START | MD_OP_WRITE | (phyaddr << MD_PMD_SHIFT)
         | (reg << MD_RA_SHIFT) | MD_TA_VALID | v);

    /* wait for it to complete */
    waitOnBit((uint *)&mdio->emacIntstatus, EI_MII, 1, 100);
}

/* Write register thru MDC/MDIO */
int switchWriteReg(struct mdioregs *mdio, uchar page, uchar reg,
                   void *val)
{
    ushort cmd16, val16;
    int i;

    cmd16 = (page << 8)         /* page number */
        | 1                     /* mdc/mdio access enable */
        ;
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_PAGE, cmd16);

    /* write data - MII register 0x18-0x1B */
    val16 = (ushort)*(ushort *)val;
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_DATA0, val16);
    val16 = (ulong)(*(ulong *)val >> 16);
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_DATA1, val16);

    /* set register address - MII register 0x11 */
    cmd16 = (reg << 8)          /* register address */
        | 1                     /* opcode write */
        ;
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_ADDR, cmd16);
    /* is operation finished? */
    for (i = MII_MAX_RETRY; i > 0; i--)
    {
        val16 = mdioRead(mdio, PSEUDO_PHYAD, REG_MII_ADDR);
        if ((val16 & 3) == 0)
            break;
    }
    return 0;
}

/* Read register thru MDC/MDIO */
int switchReadReg(struct mdioregs *mdio, uchar page, uchar reg, void *val)
{
    ushort cmd16, val16;
    int i;

    cmd16 = (page << 8)         /* page number */
        | 1                     /* mdc/mdio access enable */
        ;
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_PAGE, cmd16);

    /* set register address - MII register 0x11 */
    cmd16 = (reg << 8)          /* register address */
        | 2                     /* opcode read */
        ;
    mdioWrite(mdio, PSEUDO_PHYAD, REG_MII_ADDR, cmd16);
    /* is operation finished? */
    for (i = MII_MAX_RETRY; i > 0; i--)
    {
        val16 = mdioRead(mdio, PSEUDO_PHYAD, REG_MII_ADDR);
        if ((val16 & 3) == 0)
            break;
    }
    /* read data - MII register 0x18-0x1B */
    val16 = mdioRead(mdio, PSEUDO_PHYAD, REG_MII_DATA0);
    *(ulong *)val = val16;
    val16 = mdioRead(mdio, PSEUDO_PHYAD, REG_MII_DATA1);
    *(ulong *)val += val16 << 16;
    return 0;
}
