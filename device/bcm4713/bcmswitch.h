/*
 * @file bcmswitch.h
 *
 * $Id: bcmswitch.h 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#define SWT_PORTS 6
#define SWT_CPUPORT (SWT_PORTS - 1)

struct mdioregs
{
    ulong mdioControl;          /**< 0x410 EMAC MDIO Control         */
    ulong mdioData;
    ulong emacIntmask;
    ulong emacIntstatus;
};

struct swtstate
{
    struct mdioregs *mdio;
    uchar page;
};

extern struct swtstate swtregs;

#define SWT_CONTROL_ENABLE 0x0001

#define SWT_ADDRESS_MASK   0xFFFF
#define SWT_ADDRESS_SHIFT  0
#define SWT_ADDRESS_REG    0x00FF
#define SWT_ADDRESS_PAGE   0xFF00
#define SWT_ADDRESS_PSHIFT 8

#define SWT_SIZE_MASK      0xFF
#define SWT_SIZE_SHIFT     16
#define SWT_SIZE_8BIT      0x01
#define SWT_SIZE_16BIT     0x03
#define SWT_SIZE_32BIT     0x0F
#define SWT_SIZE_64BIT     0x3F

#define SWT_COMMAND_MASK   0xFF
#define SWT_COMMAND_SHIFT  24
#define SWT_COMMAND_WRITE  0x01
#define SWT_COMMAND_READ   0x02
#define SWT_COMMAND_BUSY   0xC0

/* Switch control space pages and registers */
#define SWT_PAGE_VLAN      0x34
#define  SWT_REG_VLAN_CONTROL0  0x00
#define  SWT_REG_VLAN_CONTROL1  0x01
#define  SWT_REG_VLAN_CONTROL2  0x02
#define  SWT_REG_VLAN_CONTROL3  0x03
#define  SWT_REG_VLAN_CONTROL4  0x04
#define  SWT_REG_VLAN_CONTROL5  0x05
#define  SWT_REG_VLAN_TABACC    0x06
#define  SWT_REG_VLAN_WRITE     0x08
#define  SWT_REG_VLAN_READ      0x0C
#define  SWT_REG_VLAN_TAGPORT0  0x10
#define  SWT_REG_VLAN_TAGPORT1  0x12
#define  SWT_REG_VLAN_TAGPORT2  0x14
#define  SWT_REG_VLAN_TAGPORT3  0x16
#define  SWT_REG_VLAN_TAGPORT4  0x18
#define  SWT_REG_VLAN_TAGMII    0x1A

#define  SWT_VLAN_TABACC_WRITE  (1 << 12)
#define  SWT_VLAN_TABACC_ENABLE (1 << 13)

#define  SWT_VLAN_WRITE_VALID   (1 << 20)

#define  SWT_VLAN_GROUPMASK     0x003F
#define  SWT_VLAN_UNTAGMASK     0x0FC0

#define  SWT_VLAN_TABLEMAX      16

int switchInit(struct bcm4713 *);
int switchWriteReg(struct mdioregs *, uchar, uchar, void *);
int switchReadReg(struct mdioregs *, uchar, uchar, void *);
