/**
 * @file bcm4713.h
 *
 * $Id: bcm4713.h 2115 2009-11-03 02:31:19Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _BCM4713_H_
#define _BCM4713_H_

#include <device.h>
#include <stddef.h>

#include <sbconfig.h>

/* Ring buffer sizes */
#define ETH_RX_RING_ENTRIES 511 /**< Number of buffers on Rx Ring       */
#define ETH_TX_RING_ENTRIES 511 /**< Number of buffers on Tx Ring       */

/**
 * Control and Status register layout for the Broadcom 4713 Ethernet
 * device.
 */
struct bcm4713
{
    volatile uint devControl;         /**< 0x000 Device control        */
#define DEVCTRL_PFE        0x00000080   /* Pattern Filtering Enable      */
#define DEVCTRL_IPP        0x00000400   /* Internal EPHY Present         */
#define DEVCTRL_EPR        0x00008000   /* EPHY Reset                    */
#define DEVCTRL_PME        0x00001000   /* PHY Mode Enable               */
#define DEVCTRL_PMCE       0x00002000   /* PHY Mode Clocks Enable        */
#define DEVCTRL_PADDR      0x0007c000   /* PHY Address                   */

    volatile uint pad0[7];

    volatile uint interruptStatus;    /**< 0x020 Interrupt Status       */
#define ISTAT_PME          0x00000040   /* Power Management Event         */
#define ISTAT_TO           0x00000080   /* General Purpose Timeout        */
#define ISTAT_DSCE         0x00000400   /* Descriptor Error               */
#define ISTAT_DATAE        0x00000800   /* Data Error                     */
#define ISTAT_DPE          0x00001000   /* Descr. Protocol Error          */
#define ISTAT_RDU          0x00002000   /* Receive Descr. Underflow       */
#define ISTAT_RFO          0x00004000   /* Receive FIFO Overflow          */
#define ISTAT_TFU          0x00008000   /* Transmit FIFO Underflow        */
#define ISTAT_RX           0x00010000   /* RX Interrupt                   */
#define ISTAT_TX           0x01000000   /* TX Interrupt                   */
#define ISTAT_EMAC         0x04000000   /* EMAC Interrupt                 */
#define ISTAT_MII_WRITE    0x08000000   /* MII Write Interrupt            */
#define ISTAT_MII_READ     0x10000000   /* MII Read Interrupt             */
#define ISTAT_ERRORS (ISTAT_DSCE | ISTAT_DATAE | ISTAT_DPE | ISTAT_RDU \
		| ISTAT_RFO | ISTAT_TFU)

    volatile uint interruptMask;      /**< Interrupt Mask               */
#define IMASK_DEF      (ISTAT_ERRORS | ISTAT_RX | ISTAT_TO | ISTAT_TX)

    volatile uint gpTimer;            /**< General Purpose Timer        */
    volatile uint pad1[31];

    volatile uint macControl;         /**< 0x0A8 MAC Control            */
#define MAC_CTRL_CRC32_ENAB 0x0000001   /* CRC32 Generation Enable        */
#define MAC_CTRL_PHY_PDOWN 0x00000004   /* Onchip EPHY Powerdown          */
#define MAC_CTRL_PHY_EDET  0x00000008   /* Onchip EPHY Energy Detected    */
#define MAC_CTRL_PHY_LEDCTRL 0x0000e0   /* Onchip EPHY LED Control        */

    volatile uint pad2[21];

    volatile uint rcvLazy;            /**< 0x100 Lazy Interrupt Control */
#define RCV_LAZY_TO_MASK   0x00ffffff   /* Timeout                        */
#define RCV_LAZY_FC_MASK   0xff000000   /* Frame Count                    */
#define RCV_LAZY_FC_SHIFT  24

    volatile uint pad3[63];

    /* Direct Memory Access (DMA) features 0x200 - 0x2FF                */
    volatile uint dmaTxControl;       /**< DMA Tx Control Register      */
#define DMATX_CTRL_ENABLE  0x00000001   /* Enable                        */
#define DMATX_CTRL_SUSPEND 0x00000002   /* Suspend Request               */
#define DMATX_CTRL_LPBACK  0x00000004   /* Loopback Enable               */
#define DMATX_CTRL_FAIRPRIO 0x00000008  /* Fair Priority                 */
#define DMATX_CTRL_FLUSH   0x00000010   /* Flush Request                 */

    volatile uint dmaTxAddress;       /**< DMA Tx Descriptor Ring       */
    volatile uint dmaTxLast;          /**< DMA Tx Last Posted Desc      */
    volatile uint dmaTxStatus;        /**< DMA Tx Current & Status      */
#define DMATX_STAT_CDMASK    0x00000fff /* Current Descriptor Mask      */
#define DMATX_STAT_SMASK     0x0000f000 /* State Mask                   */
#define DMATX_STAT_SDISABLED 0x00000000 /* State Disabled               */
#define DMATX_STAT_SACTIVE   0x00001000 /* State Active                 */
#define DMATX_STAT_SIDLE     0x00002000 /* State Idle Wait              */
#define DMATX_STAT_SSTOPPED  0x00003000 /* State Stopped                */
#define DMATX_STAT_SSUSP     0x00004000 /* State Suspend Pending        */
#define DMATX_STAT_EMASK     0x000f0000 /* Error Mask                   */
#define DMATX_STAT_ENONE     0x00000000 /* Error None                   */
#define DMATX_STAT_EDPE      0x00010000 /* Error Desc. Protocol Error   */
#define DMATX_STAT_EDFU      0x00020000 /* Error Data FIFO Underrun     */
#define DMATX_STAT_EBEBR     0x00030000 /* Bus Error on Buffer Read     */
#define DMATX_STAT_EBEDA     0x00040000 /* Bus Error on Desc Access     */
#define DMATX_STAT_FLUSHED   0x00100000 /* Flushed                      */

    volatile uint dmaRxControl;       /**< DMA Rx Control Register      */
#define DMARX_CTRL_ENABLE  0x00000001   /* Enable                         */
#define DMARX_CTRL_ROMASK  0x000000fe   /* Receive Offset Mask            */
#define DMARX_CTRL_ROSHIFT 1    /* Receive Offset Shift           */

    volatile uint dmaRxAddress;       /**< DMA Rx Descriptor Ring       */
    volatile uint dmaRxLast;          /**< DMA Rx Last Posted Desc      */

    volatile uint dmaRxStatus;        /**< DMA Rx Current & Status      */
#define DMARX_STAT_CDMASK    0x00000fff /* Current Descriptor Mask     */
#define DMARX_STAT_SMASK     0x0000f000 /* State Mask                  */
#define DMARX_STAT_SDISABLED 0x00000000 /* State Disabled              */
#define DMARX_STAT_SACTIVE   0x00001000 /* State Active                */
#define DMARX_STAT_SIDLE     0x00002000 /* State Idle Wait             */
#define DMARX_STAT_SSTOPPED  0x00003000 /* State Stopped               */
#define DMARX_STAT_EMASK     0x000f0000 /* Error Mask                  */
#define DMARX_STAT_ENONE     0x00000000 /* Error None                  */
#define DMARX_STAT_EDPE      0x00010000 /* Error Desc. Protocol Error  */
#define DMARX_STAT_EDFO      0x00020000 /* Error Data FIFO Overflow    */
#define DMARX_STAT_EBEBW     0x00030000 /* Bus Error - Buffer Write    */
#define DMARX_STAT_EBEDA     0x00040000 /* Bus Error - Desc. Access    */

    volatile uint pad4[56];

    /* Don't know what's in blocks 0x300 - 0x3FF                        */
    volatile uint pad5[64];

    /* EMAC features in block 0x400 - 0x4FF                             */
    volatile uint rxConfig;           /**< 0x400 EMAC Rx config         */
#define RXCONFIG_NONE        0x00000000 /* No Receive configuration    */
#define RXCONFIG_DBCAST      0x00000001 /* Disable Broadcast           */
#define RXCONFIG_ALLMULTI    0x00000002 /* Accept All Multicast        */
#define RXCONFIG_NORX_W_TX   0x00000004 /* Receive Disable While Tx    */
#define RXCONFIG_PROMISC     0x00000008 /* Promiscuous Enable          */
#define RXCONFIG_LPBACK      0x00000010 /* Loopback Enable             */
#define RXCONFIG_FLOW        0x00000020 /* Flow Control Enable         */
#define RXCONFIG_FLOW_ACCEPT 0x00000040 /* Accept Unicast Flow Control */
#define RXCONFIG_RFILT       0x00000080 /* Reject Filter               */

    volatile uint rxMaxLength;        /**< EMAC Rx Max Packet Length    */
    volatile uint txMaxLength;        /**< EMAC Tx Max Packet Length    */
    volatile uint pad6;
    volatile uint mdioControl;        /**< EMAC MDIO Control            */
#define MDIO_CTRL_FREQ     0x0d /* MDC Frequency                  */
#define MDIO_CTRL_FREQ_MSK 0x0000007f   /* MDC Frequency Mask             */
#define MDIO_CTRL_PREAMBLE 0x00000080   /* MII Preamble Enable            */
    volatile uint mdioData;
    volatile uint emacIntmask;
    volatile uint emacIntstatus;
    volatile uint camDataLo;          /**< EMAC CAM Data Low            */
    volatile uint camDataHi;          /**< EMAC CAM Data High           */
#define CAM_DATA_HI_VALID  0x00010000   /* Valid Bit                      */

    volatile uint camControl;         /**< EMAC CAM Control             */
#define CAM_CTRL_ENABLE    0x00000001   /* CAM Enable                     */
#define CAM_CTRL_MSEL      0x00000002   /* Mask Select                    */
#define CAM_CTRL_READ      0x00000004   /* Read                           */
#define CAM_CTRL_WRITE     0x00000008   /* Read                           */
#define CAM_CTRL_INDEX_MASK 0x03f0000   /* Index Mask                     */
#define CAM_CTRL_INDEX_SHIFT   16
#define CAM_CTRL_BUSY      0x80000000   /* CAM Busy                       */

    volatile uint enetControl;        /**< EMAC ENET Control            */
#define ENET_CTRL_ENABLE   0x00000001   /* EMAC Enable                    */
#define ENET_CTRL_DISABLE  0x00000002   /* EMAC Disable                   */
#define ENET_CTRL_SRST     0x00000004   /* EMAC Soft Reset                */
#define ENET_CTRL_EPSEL    0x00000008   /* External PHY Select            */

    volatile uint pad7;
    volatile uint txWatermark;        /**< EMAC Tx Watermark            */
#define TX_WATERMARK         48 /* Sane value for watermark       */
    volatile uint mibControl;         /**< EMAC MIB Control             */
#define MIB_CTRL_CLR_ON_READ 0x000001   /* Autoclear on Read              */

    volatile uint pad8[49];

    /* MIB statistics in block 0x500 - 0x5FF                            */
    volatile uint txGoodOctets;       /**< MIB Tx Good Octets           */
    volatile uint txGoodPackets;      /**< MIB Tx Good Packets          */
    volatile uint txOctets;           /**< MIB Tx Octets                */
    volatile uint txPackets;          /**< MIB Tx Packets               */
    volatile uint txBroadcast;        /**< MIB Tx Broadcast Packets     */
    volatile uint txMulticast;        /**< MIB Tx Multicast Packets     */
    volatile uint tx_64;              /**< MIB Tx <= 64 byte Packets    */
    volatile uint tx_65_127;          /**< MIB Tx 65 to 127 bytes       */
    volatile uint tx_128_255;         /**< MIB Tx 128 to 255 bytes      */
    volatile uint tx_256_511;         /**< MIB Tx 256 to 511 bytes      */
    volatile uint tx_512_1023;        /**< MIB Tx 512 to 1023 bytes     */
    volatile uint tx_1024_max;        /**< MIB Tx 1024 to max bytes     */
    volatile uint txJabber;           /**< MIB Tx Jabber Packets        */
    volatile uint txOversize;         /**< MIB Tx Oversize Packets      */
    volatile uint txFragment;         /**< MIB Tx Fragment Packets      */
    volatile uint txUnderruns;        /**< MIB Tx Underruns             */
    volatile uint txCollisions;       /**< MIB Tx Total Collisions      */
    volatile uint txCollSingle;       /**< MIB Tx Single Collisions     */
    volatile uint txCollMultiple;     /**< MIB Tx Multiple Collision    */
    volatile uint txCollExcess;       /**< MIB Tx Excessive Collision   */
    volatile uint txCollLate;         /**< MIB Tx Late Collisions       */
    volatile uint txDeferred;         /**< MIB Tx Deferred Packets      */
    volatile uint txLostCarrier;      /**< MIB Tx Carrier Lost          */
    volatile uint txPause;            /**< MIB Tx Pause Packets         */
    volatile uint pad9[8];
    volatile uint rxGoodOctets;       /**< MIB Rx Good Octets           */
    volatile uint rxGoodPackets;      /**< MIB Rx Good Packets          */
    volatile uint rxOctets;           /**< MIB Rx Octets                */
    volatile uint rxPackets;          /**< MIB Rx Packets               */
    volatile uint rxBroadcast;        /**< MIB Rx Broadcast Packets     */
    volatile uint rxMulticast;        /**< MIB Rx Multicast Packets     */
    volatile uint rx_64;              /**< MIB Rx <= 64 byte Packets    */
    volatile uint rx_65_127;          /**< MIB Rx 65 to 127 bytes       */
    volatile uint rx_128_255;         /**< MIB Rx 128 to 255 bytes      */
    volatile uint rx_256_511;         /**< MIB Rx 256 to 511 bytes      */
    volatile uint rx_512_1023;        /**< MIB Rx 512 to 1023 bytes     */
    volatile uint rx_1024_max;        /**< MIB Rx 1024 to max bytes     */
    volatile uint rxJabber;           /**< MIB Rx Jabber Packets        */
    volatile uint rxOversize;         /**< MIB Rx Oversize Packets      */
    volatile uint rxFragment;         /**< MIB Rx Fragment Packets      */
    volatile uint rxMissed;           /**< MIB Rx Missed Packets        */
    volatile uint rxCrcAlign;         /**< MIB Rx CRC Align Errors      */
    volatile uint rxUndersize;        /**< MIB Rx Undersize Packets     */
    volatile uint rxCrc;              /**< MIB Rx CRC Errors            */
    volatile uint rxAlign;            /**< MIB Rx Align Errors          */
    volatile uint rxSymbol;           /**< MIB Rx Symbol Errors         */
    volatile uint rxPause;            /**< MIB Rx Pause Packets         */
    volatile uint rxNonPause;         /**< MIB Rx Non-Pause Packets     */
    volatile uint pad10[9];

    /* Don't know what's in blocks 0x600 - 0xEFF                        */
    volatile uchar pad11[256 * 9];

    /* Silicon Backplane configuration space in 0xF00 - 0xFFF           */
    volatile struct backplaneConfig bpConfig; /**< configuration space  */
};

/* Reciever Header struct and constants */
#define ETH_RX_FLAG_OFIFO   0x0001 /**< FIFO Overflow                   */
#define ETH_RX_FLAG_CRCERR  0x0002 /**< CRC Error                       */
#define ETH_RX_FLAG_SERR    0x0004 /**< Receive Symbol Error            */
#define ETH_RX_FLAG_ODD     0x0008 /**< Frame has odd number nibbles    */
#define ETH_RX_FLAG_LARGE   0x0010 /**< Frame is > RX MAX Length        */
#define ETH_RX_FLAG_MCAST   0x0020 /**< Dest is Multicast Address       */
#define ETH_RX_FLAG_BCAST   0x0040 /**< Dest is Broadcast Address       */
#define ETH_RX_FLAG_MISS    0x0080 /**< Received due to promisc mode    */
#define ETH_RX_FLAG_LAST    0x0800 /**< Last buffer in frame            */
#define ETH_RX_FLAG_ERRORS  ( ETH_RX_FLAG_ODD | ETH_RX_FLAG_SERR | \
                              ETH_RX_FLAG_CRCERR | ETH_RX_FLAG_OFIFO )

/**
 * Header on a received packet.
 */
struct rxHeader
{
    ushort length;              /**< Length of packet data              */
    ushort flags;               /**< Receive flags                      */
    ushort pad[12];             /**< Padding                            */
};

/* Ethernet DMA descriptor */
#define ETH_DESC_CTRL_LEN   0x00001fff /**< Mask for length field       */
#define ETH_DESC_CTRL_CMASK 0x0ff00000 /**< Core specific bits          */
#define ETH_DESC_CTRL_EOT   0x10000000 /**< End of Table                */
#define ETH_DESC_CTRL_IOC   0x20000000 /**< Interrupt On Completion     */
#define ETH_DESC_CTRL_EOF   0x40000000 /**< End of Frame                */
#define ETH_DESC_CTRL_SOF   0x80000000 /**< Start of Frame              */

/**
 * Descriptor for the DMA engine to determine where to find a packet
 * buffer.
 */
struct dmaDescriptor
{
    ulong control;              /**< DMA control bits                   */
    ulong address;              /**< Stored as physical address         */
};

#endif                          /* _BCM4713_H_ */
