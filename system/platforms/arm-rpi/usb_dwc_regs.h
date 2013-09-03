/**
 * @file usb_dwc_regs.h
 *
 * Registers of the DesignWare Hi-Speed USB 2.0 On-The-Go Controller.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _USB_DWC_REGS_H_
#define _USB_DWC_REGS_H_

#include <usb_util.h>

/**
 * Number of DWC host channels, each of which can be used for an independent
 * USB transfer.  On the BCM2835 (Raspberry Pi), 8 are available.  This is
 * documented on page 201 of the BCM2835 ARM Peripherals document.
 */
#define DWC_NUM_CHANNELS 8

/**
 * Layout of the registers of the DesignWare Hi-Speed USB 2.0 On-The-Go
 * Controller.  There is no official documentation for these; however, the
 * register locations (and to some extent the meanings) can be found in other
 * code, such as the Linux driver for this hardware that Synopsys contributed.
 *
 * We do not explicitly define every bit in the registers because the majority
 * are not used by our driver and would complicate this file.  For example, we
 * do not attempt to document any features that are specific to suspend,
 * hibernation, the OTG protocol, or to the core acting in device mode rather
 * than host mode.
 *
 * The bits and fields we do use in our driver we have tried to completely
 * document based on our understanding of what they do.  We cannot guarantee
 * that all the information is correct, as we do not have access to any official
 * documentation.
 */
struct dwc_regs {

    /* 0x000 : OTG Control */
    u32 otg_control;

    /* 0x004 : OTG Interrupt */
    u32 otg_interrupt;

    /**
     * 0x008 : AHB Configuration Register.
     *
     * This register configures some of the interactions the DWC has with the
     * rest of the system.  */
    u32 ahb_configuration;

/** Enable interrupts from the USB controller.  Disabled by default.  */
#define DWC_AHB_INTERRUPT_ENABLE  (1 << 0)

/**
 * Bits [4:1] of the AHB Configuration register were redefined by Broadcom for
 * the BCM2835; hence this flag is only valid on the BCM2835.
 *
 * This bit is documented as:
 *
 *   1 = Wait for all outstanding AXI writes to complete before signalling
 *      (internally) that DMA is done.
 *   0 = don't wait.
 *
 * We set this bit because the Linux driver does, although we did not observe a
 * difference in behavior.
 */
#define BCM_DWC_AHB_AXI_WAIT      (1 << 4)

/**
 * Writing 1 to this bit in the AHB Configuration Register allows the USB
 * controller to perform DMA (Direct Memory Access).  Disabled by default.
 */
#define DWC_AHB_DMA_ENABLE        (1 << 5)

    /* 0x01c : Core USB configuration */
    u32 core_usb_configuration;


    /**
     * 0x010 : Core Reset Register.
     *
     * Software can use this register to cause the DWC to reset itself.
     */
    u32 core_reset;

    /**
     * TODO
     */
#define DWC_AHB_MASTER_IDLE (1 << 31)

    /**
     * Write 1 to this location in the Core Reset Register to start a soft
     * reset.  This bit will then be cleared by the hardware when the reset is
     * complete.
     */
#define DWC_SOFT_RESET      (1 << 0)

    /**
     * 0x014 : Core Interrupt Register.
     *
     * This register contains the state of pending top-level DWC interrupts.  1
     * means interrupt pending while 0 means no interrupt pending.
     *
     * Note that at least for port_intr and host_channel_intr, software must
     * clear the interrupt somewhere else rather than by writing to this
     * register.
     */
    union dwc_core_interrupts {
        u32 val;
        struct {
            u32 stuff             : 3;

            /**
             * Start of Frame.  TODO
             */
            u32 sof_intr          : 1;

            u32 morestuff         : 20;

            /**
             * Host port status changed.  Software must examine the Host Port
             * Control and Status Register to determine the current status of
             * the host port and clear any flags in it that indicate a status
             * change.
             */
            u32 port_intr         : 1;     /* Bit 24 */

            /**
             * Channel interrupt occurred.  Software must examine the Host All
             * Channels Interrupt Register to determine which channel(s) have
             * pending interrupts, then handle and clear the interrupts for
             * these channels.
             */
            u32 host_channel_intr : 1;     /* Bit 25 */

            u32 evenmorestuff     : 6;
        };
    } core_interrupts;

    /**
     * 0x018 : Core Interrupt Mask Register.
     *
     * This register has the same format as the Core Interrupt Register and
     * configures whether the corresponding interrupt is enabled (1) or disabled
     * (0).  Initial state after reset is all 0's.
     */
    union dwc_core_interrupts core_interrupt_mask;

    /* 0x01c : Receive Status Queue Read */
    u32 receive_status;

    /* 0x020 : Receive Status Queue Read & Pop */
    u32 receive_status_pop;

    /**
     * 0x024 : Receive FIFO Size Register.
     *
     * This register contains the size of the Receive FIFO, in 4-byte words.
     *
     * This register must be set by software before using the controller; see
     * the note in the documentation for the hwcfg3 register about configuring
     * the dynamic FIFOs.
     */
    u32 rx_fifo_size;

    /**
     * 0x028 : Non Periodic Transmit FIFO Size Register.
     *
     * The low 16 bits of this register contain the offset of the Nonperiodic
     * Transmit FIFO, in 4-byte words, from the start of the memory reserved by
     * the controller for dynamic FIFOs.  The high 16 bits of this register
     * contain its size, in 4-byte words.
     *
     * This register must be set by software before using the controller; see
     * the note in the documentation for the hwcfg3 register about configuring
     * the dynamic FIFOs.
     */
    u32 nonperiodic_tx_fifo_size;

    /* 0x02c : Non Periodic Transmit FIFO/Queue Status Register */
    u32 nonperiodic_tx_fifo_status;

    /* 0x030 */
    u32 i2c_control;

    /* 0x034 */
    u32 phy_vendor_control;

    /* 0x038 */
    u32 gpio;

    /* 0x03c */
    u32 user_id;

    /* 0x040 */
    u32 vendor_id;

    /* 0x044 */
    u32 hwcfg1;

    /* 0x048 */
    u32 hwcfg2;

    /**
     * 0x04c : Hardware Configuration 3 Register.
     *
     * The high 16 bits of this read-only register contain the maximum total
     * size, in words, of the dynamic FIFOs (Rx, Nonperiodic Tx, and Periodic
     * Tx).  Software must set up these three dynamic FIFOs in the rx_fifo_size,
     * nonperiodic_tx_fifo_size, and host_periodic_tx_fifo_size registers such
     * that their total size does not exceed this maximum total size and no
     * FIFOs overlap.
     *
     * Note: Software must explicitly configure the dynamic FIFOs even if the
     * controller is operating in DMA mode, since the default values for the
     * FIFO sizes and offsets may be invalid.  For example, in Broadcom's
     * instantiation of this controller for the BCM2835, only 4080 words are
     * available for dynamic FIFOs, but the dynamic FIFO sizes are set to 4096,
     * 32, and 0, which are invalid as they add up to more than 4080.  <b>IF YOU
     * DO NOT DO THIS YOU WILL GET SILENT MEMORY CORRUPTION</b>.
     *
     * The low 16 bits of this register contain various flags that are not
     * documented here as we don't use any in our driver.
     */
    u32 hwcfg3;

    /* 0x050 */
    u32 hwcfg4;

    /* 0x054 */
    u32 core_lpm_configuration;

    /* 0x058 */
    u32 global_powerDn;

    /* 0x05c */
    u32 global_fifo_config;

    /* 0x060 */
    u32 adp_control;

    /* 0x064 */
    u32 reserved_0x64[39];

    /**
     * 0x100 : Host Periodic Transmit FIFO Size Register.
     *
     * The low 16 bits of this register configure the offset of the Periodic
     * Transmit FIFO, in 4-byte words, from the start of the memory reserved by
     * the controller for dynamic FIFOs.  The high 16 bits of this register
     * configure its size, in 4-byte words.
     *
     * This register should be set by software before using the controller; see
     * the note in the documentation for the hwcfg3 register about configuring
     * the dynamic FIFOs.
     */
    u32 host_periodic_tx_fifo_size;

    /* TODO */
    u32 stuff[191];

    /** 
     * @name Host registers
     *
     * The registers beginning at this point are considered to be the "Host"
     * registers.  These are used for the "Host" half of the OTG (On-The-Go)
     * protocol, which allows this hardware to act as either a USB host or a USB
     * device.  This is the only half we are concerned with in this driver and
     * we do not declare the corresponding Device registers.  */
    /**@{*/

    /* 0x400 */
    u32 host_configuration;

    /* 0x404 */
    u32 host_frame_interval;

    /* 0x408 */
    u32 host_frame_number;

    /* 0x40c */
    u32 host_reserved_0x40c;

    /* 0x410 */
    u32 host_fifo_status;

    /**
     * 0x414 : Host All Channels Interrupt Register.
     *
     * This register contains a bit for each host channel that indicates whether
     * an interrupt has occurred on that host channel.  You cannot clear the
     * interrupts by writing to this register; use the channel-specific
     * interrupt registers instead.
     */
    u32 host_channels_interrupt;

    /**
     * 0x418 : Host All Channels Interrupt Mask Register.
     *
     * Same format as the Host All Channels Interrupt Register, but a 1 in this
     * register indicates that the corresponding host channel interrupt is
     * enabled.  Software can change this register.  Defaults to all 0's after a
     * reset.
     */
    u32 host_channels_interrupt_mask;

    /* 0x41c */
    u32 host_frame_list;

    /* 0x420 */
    u32 host_reserved_0x420[8];

    /**
     * 0x440 : Host Port Control and Status Register.
     *
     * This register provides the information needed to respond to status
     * queries about the "host port", which is the port that is logically
     * attached to the root hub.
     *
     * When changing this register, software must read its value, then clear the
     * enabled, connected_changed, enabled_changed, and overcurrent_changed
     * members to avoid changing them, as those particular bits are cleared by
     * writing 1.
     */
    union dwc_host_port_ctrlstatus {
        u32 val;
        struct {
            /**
             *  1: a device is connected to this port.
             *  0: no device is connected to this port.
             *
             *  Changed by hardware only.
             */
            u32 connected : 1; /* Bit 0 */

            /**
             * Set by hardware when connected bit changes.  Software can write
             * 1 to acknowledge and clear.  The setting of this bit by hardware
             * generates an interrupt that can be enabled by setting port_intr
             * in the core_interrupt_mask register.
             */
            u32 connected_changed : 1; /* Bit 1 */

            /**
             * 1: port is enabled.
             * 0: port is disabled.
             *
             * Note: the host port is enabled by default after it is reset.
             *
             * Note: Writing 1 here appears to disable the port.
             */
            u32 enabled : 1; /* Bit 2 */

            /**
             * Set by hardware when enabled bit changes.  Software can write 1
             * to acknowledge and clear.  The setting of this bit by hardware
             * generates an interrupt that can be enabled by setting port_intr
             * in the core_interrupt_mask register.
             */
            u32 enabled_changed : 1; /* Bit 3 */

            /**
             * 1: overcurrent condition active on this port
             * 0: no overcurrent condition active on this port
             *
             * Changed by hardware only.
             */
            u32 overcurrent : 1; /* Bit 4 */

            /**
             * Set by hardware when the overcurrent bit changes.  The software
             * can write 1 to acknowledge and clear.  The setting of this bit by
             * hardware generates the interrupt that can be enabled by setting
             * port_intr in the core_interrupt_mask register.
             */
            u32 overcurrent_changed : 1; /* Bit 5 */

            /**
             * Set by software to set resume signalling.
             */
            u32 resume : 1; /* Bit 6 */

            /**
             * Set by software to suspend the port.
             */
            u32 suspended : 1; /* Bit 7 */

            /**
             * Software can set this to start a reset on this port.  Software
             * must clear this after waiting 60 milliseconds for the reset is
             * complete.
             */
            u32 reset : 1; /* Bit 8 */

            u32 reserved : 1; /* Bit 9 */

            /**
             * Current logic of data lines (10: logic of D+; 11: logic of D-).
             *
             * Changed by hardware only.
             */
            u32 line_status : 2; /* Bits 10-11*/

            /**
             * 1: port is powered.
             * 0: port is not powered.
             *
             * Software can change this bit to power on (1) or power off (0)
             * the port.
             */
            u32 powered : 1; /* Bit 12 */

            u32 test_control : 4; /* Bits 13-16 */

            /**
             * Speed of attached device (if any).  This should only be
             * considered meaningful if the connected bit is set.
             *
             * 00: high speed; 01: full speed; 10: low speed
             *
             * Changed by hardware only.
             */
            u32 speed : 2; /* Bits 17-18 */

            u32 reserved2 : 13; /* Bits 19-32 */

        };
    } host_port_ctrlstatus;

    u32 host_reserved_0x444[47];

    /**
     * 0x500 : Array of host channels.  Each host channel can be used to
     * execute an independent USB transfer or transaction simultaneously.  A USB
     * transfer may consist of multiple transactions, or packets.  To avoid
     * having to re-program the channel, it may be useful to use one channel for
     * all transactions of a transfer before allowing other transfers to be
     * scheduled on it.
     */
    struct dwc_host_channel {

        /**
         * Channel Characteristics Register -
         *
         * Contains various fields that must be set to prepare this channel for
         * a transfer to or from a particular endpoint on a particular USB
         * device.
         *
         * This register only needs to be programmed one time when doing a
         * transfer, regardless of how many packets it consists of, unless the
         * channel is re-programmed for a different transfer or the transfer is
         * moved to a different channel.
         */
        union dwc_host_channel_characteristics {
            u32 val;
            struct {
                /**
                 * Maximum packet size the endpoint is capable of sending or
                 * receiving.  Must be programmed by software before starting
                 * the transfer.
                 */
                u32 max_packet_size     : 11; /* Bits 0-10  */

                /**
                 * Endpoint number (low 4 bits of bEndpointAddress).  Must be
                 * programmed by software before starting the transfer.
                 */
                u32 endpoint_number     : 4;  /* Bits 11-14 */

                /**
                 * Endpoint direction (high bit of bEndpointAddress).  Must be
                 * programmed by software before starting the transfer.
                 */
                u32 endpoint_direction  : 1;  /* Bit  15    */

                u32 reserved            : 1;  /* Bit  16    */

                /**
                 * 1 when the device being communicated with is attached at low
                 * speed; 0 otherwise.  Must be programmed by software before
                 * starting the transfer.
                 */
                u32 low_speed           : 1;  /* Bit  17    */

                /**
                 * Endpoint type (low 2 bits of bmAttributes).  Must be
                 * programmed by software before starting the transfer.
                 */
                u32 endpoint_type       : 2;  /* Bits 18-19 */

                /**
                 * Maximum number of transactions that can be executed per
                 * microframe as part of this transfer.  Normally 1, but should
                 * be set to 1 + (bits 11 and 12 of wMaxPacketSize) for
                 * high-speed interrupt and isochronous endpoints.  Must be
                 * programmed by software before starting the transfer.
                 */
                u32 packets_per_frame   : 2;  /* Bits 20-21 */

                /**
                 * USB device address of the device on which the endpoint is
                 * located.  Must be programmed by software before starting the
                 * transfer.
                 */
                u32 device_address      : 7;  /* Bits 22-28 */

                /**
                 * Just before enabling the channel (for all transactions),
                 * software needs to set this to the opposite of the low bit of
                 * the host_frame_number register.  Otherwise the hardware will
                 * issue frame overrun errors on some transactions.  TODO: what
                 * exactly does this do?
                 */
                u32 odd_frame           : 1;  /* Bit  29    */

                /**
                 * Software can set this to 1 to halt the channel.  Not needed
                 * during normal operation as the channel halts automatically
                 * when a transaction completes or an error occurs.
                 */
                u32 channel_disable     : 1;  /* Bit  30    */

                /**
                 * Software can set this to 1 to enable the channel, thereby
                 * actually starting the transaction on the USB.  This must only
                 * be done after the characteristics, split_control, and
                 * transfer registers, and possibly other registers (depending
                 * on the transfer) have been programmed.
                 */
                u32 channel_enable      : 1;  /* Bit  31    */
            };
        } characteristics;

        /**
         * Channel Split Control Register -
         *
         * This register is used to set up Split Transactions for communicating
         * with low or full-speed devices attached to a high-speed hub.  When
         * doing so, set split_enable to 1 and the other fields as documented.
         * Otherwise, software must clear this register before starting the
         * transfer.
         *
         * Like the Channel Characteristics register, this register only
         * needs to be programmed one time if the channel is enabled multiple
         * times to send all the packets of a single transfer.
         */
        union dwc_host_channel_split_control {
            u32 val;
            struct {
                /**
                 * 0-based index of the port on the high-speed hub on which the
                 * low or full-speed device is attached.
                 */
                u32 port_address          : 7;  /* Bits 0-6   */

                /**
                 * USB device address of the high-speed hub that acts as the
                 * Transaction Translator for this low or full-speed device.
                 * This is not necessarily the hub the device is physically
                 * connected to, since that could be a full-speed or low-speed
                 * hub.  Instead, software must walk up the USB device tree
                 * (towards the root hub) until a high-speed hub is found and
                 * use its device address here.
                 */
                u32 hub_address           : 7;  /* Bits 7-13  */

                /**
                 * TODO: what exactly does this do?
                 */
                u32 transaction_position  : 2;  /* Bits 14-15 */

                /**
                 *  0: Do a Start Split transaction
                 *  1: Do a Complete Split transaction.
                 *
                 * When split transactions are enabled, this must be programmed
                 * by software before enabling the channel.  Note that you must
                 * begin with a Start Split transaction and alternate this bit
                 * for each transaction until the transfer is complete.
                 */
                u32 complete_split        : 1;  /* Bit  16    */

                u32 reserved              : 14; /* Bits 17-30 */

                /**
                 * Set to 1 to enable Split Transactions.
                 */
                u32 split_enable          : 1;  /* Bit  31    */
            };
        } split_control;

        /**
         * Channel Interrupts Register -
         *
         * Bitmask of status conditions that have occurred on this channel.
         *
         * These bits can be used with or without "real" interrupts.  To have
         * the CPU get a real interrupt when one of these bits gets set, set the
         * appropriate bit in the interrupt_mask, and also ensure that
         * interrupts from the channel are enabled in the
         * host_channels_interrupt_mask register, channel interrupts overall are
         * enabled in the core_interrupt_mask register, and interrupts from the
         * DWC hardware overall are enabled in the ahb_configuration register
         * and by any system-specific interrupt controller.
         */
        union dwc_host_channel_interrupts {
            u32 val;
            struct {
                /**
                 * The requested USB transfer has successfully completed.
                 *
                 * Exceptions and caveats:
                 *
                 * - When doing split transactions, this bit will be set after a
                 *   Complete Split transaction has finished, even though the
                 *   overall transfer may not actually be complete.
                 *
                 * - The transfer will only be complete up to the extent that
                 *   data was programmed into the channel.  For example, control
                 *   transfers have 3 phases, each of which must be programmed
                 *   into the channel separately.  This flag will be set after
                 *   each of these phases has successfully completed.
                 *
                 * - An OUT transfer is otherwise considered complete when
                 *   exactly the requested number of bytes of data have been
                 *   successfully transferred, while an IN transfer is otherwise
                 *   considered complete when exactly the requested number of
                 *   bytes of data have been successfully transferred or a
                 *   shorter-than-expected packet was received.
                 */
                u32 transfer_completed       : 1;  /* Bit 0     */

                /**
                 * The channel has halted.  After this bit has been set, the
                 * channel sits idle and nothing else will happen until software
                 * takes action.
                 *
                 * Channels may halt for several reasons.  From our experience
                 * these cover all possible situations in which software needs
                 * to take action, so this is the only channel interrupt that
                 * actually needs to be enabled.  At least in DMA mode, the
                 * controller to some extent will act autonomously to complete
                 * transfers and only issue this interrupt when software needs
                 * to take action.
                 *
                 * Situations in which a channel will halt include but probably
                 * are not limited to:
                 *
                 * - The transfer has completed, thereby setting the
                 *   transfer_completed flag as documented above.
                 *
                 * - A Start Split or Complete Split transaction has finished.
                 *
                 * - The hub sent a NYET packet when trying to execute a
                 *   Complete Split transaction, thereby signalling that the
                 *   Split transaction is not yet complete.
                 *
                 * - The device sent a NAK packet, thereby signalling it had no
                 *   data to send at the time, when trying to execute an IN
                 *   interrupt transfer.
                 *
                 * - One of several errors has occurred, such as an AHB error,
                 *   data toggle error, tranasction error, stall condition, or
                 *   frame overrun error.
                 */
                u32 channel_halted           : 1;  /* Bit 1     */

                /**
                 * An error occurred on the ARM Advanced High-Performance Bus
                 * (AHB).
                 */
                u32 ahb_error                : 1;  /* Bit 2     */

                /**
                 * The device issued a STALL handshake packet (endpoint is
                 * halted or control pipe request is not supported).
                 */
                u32 stall_response_received  : 1;  /* Bit 3     */

                /**
                 * The device issued a NAK handshake packet (receiving device
                 * cannot accept data or transmitting device cannot send data).
                 *
                 * The channel will halt with this bit set when performing an IN
                 * transfer from an interrupt endpoint that has no data to send.
                 * As this requires software intervention to restart the
                 * channel, this means that polling of interrupt endpoints (e.g.
                 * on hubs and HID devices) must be done in software, even if
                 * the actual transactions themselves are interrupt-driven.
                 */
                u32 nak_response_received    : 1;  /* Bit 4     */

                /**
                 * The device issued an ACK handshake packet (receiving device
                 * acknowledged error-free packet).
                 */
                u32 ack_response_received    : 1;  /* Bit 5     */

                /**
                 * The device issued a NYET handshake packet.
                 */
                u32 nyet_response_received   : 1;  /* Bit 6     */

                /**
                 * From our experience this seems to usually indicate that
                 * software programmed the channel incorrectly.
                 */
                u32 transaction_error        : 1;  /* Bit 7     */

                /**
                 * Unexpected bus activity occurred.
                 */
                u32 babble_error             : 1;  /* Bit 8     */

                /**
                 * TODO
                 */
                u32 frame_overrun            : 1;  /* Bit 9     */

                /**
                 * When issuing a series of DATA transactions to an endpoint,
                 * the correct DATA0 or DATA1 packet ID was not specified in the
                 * packet_id member of the transfer register.
                 */
                u32 data_toggle_error        : 1;  /* Bit 10    */

                u32 buffer_not_available     : 1;  /* Bit 11    */
                u32 excess_transaction_error : 1;  /* Bit 12    */
                u32 frame_list_rollover      : 1;  /* Bit 13    */
                u32 reserved                 : 18; /* Bits 14-31 */
            };
        } interrupts;

        /**
         * Channel Interrupts Mask Register -
         *
         * This has the same format as the Channel Interrupts Register, but
         * software uses this to enable (1) or disable (0) the corresponding
         * interrupt.  Defaults to all 0's after a reset.
         */
        union dwc_host_channel_interrupts interrupt_mask;

        /**
         * Channel Transfer Register:
         *
         * Used to store additional information about the transfer.  This must
         * be programmed before beginning the transfer.
         */
        union dwc_host_channel_transfer {
            u32 val;
            struct {
                /**
                 * Size of the data to send or receive, in bytes.  Software must
                 * program this before beginning the transfer.  This can be
                 * greater than the maximum packet length.
                 *
                 * For IN transfers, the hardware decrements this field for each
                 * packet received by the number of bytes received.  For split
                 * transactions, the decrement happens after the Complete Split
                 * rather than the Start Split.  Software can subtract this
                 * field from the original transfer size in order to determine
                 * the number of bytes received at any given point, including
                 * when the transfer has encountered an error or has completed
                 * with either the full size or a short size.
                 *
                 * For OUT transfers, the hardware does not update this field as
                 * expected.  It will not be decremented when data is
                 * transmitted, at least not in every case; hence, software
                 * cannot rely on its value to indicate how many bytes of data
                 * have been transmitted so far.  Instead, software must inspect
                 * the packet_count field and assume that all data was
                 * transmitted if packet_count is 0, or that the amount of data
                 * transmitted is equal to the endpoint's maximum packet size
                 * times [the original packet count minus packet_count] if
                 * packet_count is nonzero.
                 */
                u32 size         : 19; /* Bits 0-18  */

                /**
                 * Number of packets left to transmit or maximum number of
                 * packets left to receive.  Software must program this before
                 * beginning the transfer.  The packet count is calculated as
                 * the size divided by the maximum packet size, rounded up to
                 * the nearest whole packet.  As a special case, if the transfer
                 * size is 0 bytes, the packet count must be set to 1.
                 *
                 * The hardware will decrement this register when a packet is
                 * successfully sent or received.  In the case of split
                 * transactions, this happens after the Complete Split rather
                 * than after the Start Split.  If the final received packet of
                 * an IN transfer is short, it is still counted.
                 */
                u32 packet_count : 10; /* Bits 19-28 */

                /**
                 * High 2 bits of the Packet ID used in the USB protocol.
                 *
                 * When performing the SETUP phase of a control transfer,
                 * specify 0x3 here to generate the needed SETUP token.
                 *
                 * When performing the DATA phase of a control transfer,
                 * initially specify 0x2 here to begin the DATA packets with the
                 * needed DATA1 Packet ID.
                 *
                 * When performing the STATUS phase of a control transfer,
                 * specify 0x2 here to generate the neeed DATA1 Packet ID.
                 *
                 * When starting a bulk, isochronous, or interrupt transfer,
                 * specify 0x0 here to generate the needed DATA0 Packet ID.
                 *
                 * In the case of a transfer consisting of multiple DATA
                 * packets, the hardware will update this field with the Packet
                 * ID to use for the next packet.  This field therefore only
                 * needs to be re-programmed if the transfer is moved to a
                 * different channel or the channel is re-used before the
                 * transfer is complete.  When doing so, software must save this
                 * field so that it can be re-programmed correctly.
                 */
                u32 packet_id    : 2;  /* Bits 29-30 */

                /**
                 * TODO
                 */
                u32 do_ping      : 1;  /* Bit  31    */
            };
        } transfer;

        /**
         * Channel DMA Address Register -
         *
         * Word-aligned address at which the hardware will read or write data
         * using Direct Memory Access.  This must be programmed before beginning
         * the transfer, unless the size of the data to send or receive is 0.
         * The hardware will increment this address by the number of bytes
         * successfully received or sent, which will correspond to the size
         * decrease in transfer.size.
         *
         * Note: DMA must be enabled in the AHB Configuration Register before
         * this register can be used.  Otherwise, the hardware is considered to
         * be in Slave mode and must be controlled a different way, which we do
         * not use in our driver and do not attempt to document.
         *
         * BCM2835-specific note:  Theoretically, addresses written to this
         * register must be bus addresses, not ARM physical addresses.  However,
         * in our experience the behavior is the same when simply using ARM
         * physical addresses.
         */
        u32 dma_address;

        u32 reserved_1;
        u32 reserved_2;
    } host_channels[DWC_NUM_CHANNELS];

    u32 host_reserved_after_channels[(0x800 - 0x500 -
                        (DWC_NUM_CHANNELS * sizeof(struct dwc_host_channel))) /
                         sizeof(u32)];

    /**@}*/

    /* 0x800 */

    u32 reserved_0x800[(0xe00 - 0x800) / sizeof(u32)];

    /* 0xe00 : Power and Clock Gating Control Register */
    u32 power;
};

/* Make sure the registers are declared correctly.  This is dummy code that will
 * be compiled into nothing.  */
static inline void _dwc_check_regs(void)
{
    STATIC_ASSERT(offsetof(struct dwc_regs, vendor_id) == 0x40);
    STATIC_ASSERT(offsetof(struct dwc_regs, host_periodic_tx_fifo_size) == 0x100);
    STATIC_ASSERT(offsetof(struct dwc_regs, host_configuration) == 0x400);
    STATIC_ASSERT(offsetof(struct dwc_regs, host_port_ctrlstatus) == 0x440);
    STATIC_ASSERT(offsetof(struct dwc_regs, reserved_0x800) == 0x800);
    STATIC_ASSERT(offsetof(struct dwc_regs, power) == 0xe00);
}

#endif /* _USB_DWC_REGS_H_ */
