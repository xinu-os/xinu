/**
 * @file tftpSendRRQ.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tftp.h>
#include <device.h>
#include <string.h>

/**
 * Send a TFTP RRQ (Read Request) packet over a UDP connection to the TFTP
 * server.  This instructs the TFTP server to begin sending the contents of the
 * specified file.  Not intended to be used outside of the TFTP code.
 *
 * @param udpdev
 *      Device descriptor for the open UDP device.
 * @param filename
 *      Name of the file to request.
 *
 * @return
 *      OK if packet sent successfully; SYSERR otherwise.
 */
syscall tftpSendRRQ(int udpdev, const char *filename)
{
    char *p;
    uint filenamelen;
    uint pktlen;
    struct tftpPkt pkt;

    /* Do sanity check on filename.  */
    filenamelen = strnlen(filename, 256);
    if (0 == filenamelen || 256 == filenamelen)
    {
        TFTP_TRACE("Filename is invalid.");
        return SYSERR;
    }

    TFTP_TRACE("RRQ \"%s\" (mode: octet)", filename);

    /* Set TFTP opcode to RRQ (Read Request).  */
    pkt.opcode = hs2net(TFTP_OPCODE_RRQ);

    /* Set up filename and mode.  */
    p = pkt.RRQ.filename_and_mode;
    memcpy(p, filename, filenamelen + 1);
    p += filenamelen + 1;
    memcpy(p, "octet", 6);
    p += 6;

    /* Write the resulting packet to the UDP device.  */
    pktlen = p - (char*)&pkt;
    if (pktlen != write(udpdev, &pkt, pktlen))
    {
        TFTP_TRACE("Error sending RRQ");
        return SYSERR;
    }
    return OK;
}
