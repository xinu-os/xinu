/**
 * @file tftpSendACK.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tftp.h>
#include <device.h>

/**
 * Send a TFTP ACK (Acknowledge) packet over a UDP connection to the TFTP
 * server.  For TFTP Get transfers, this informs the TFTP server that a given
 * data packet (having a specific block number) has been received.  Not intended
 * to be used outside of the TFTP code.
 *
 * @param udpdev
 *      Device descriptor for the open UDP device.
 * @param block_number
 *      Block number to acknowledge.
 *
 * @return
 *      OK if packet sent successfully; SYSERR otherwise.
 */
syscall tftpSendACK(int udpdev, ushort block_number)
{
    struct tftpPkt pkt;

    TFTP_TRACE("ACK block %u", block_number);
    pkt.opcode = hs2net(TFTP_OPCODE_ACK);
    pkt.ACK.block_number = hs2net(block_number);
    if (4 != write(udpdev, &pkt, 4))
    {
        TFTP_TRACE("Error sending ACK");
        return SYSERR;
    }
    return OK;
}
