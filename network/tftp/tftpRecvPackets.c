/**
 * @file tftpRecvPackets.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tftp.h>
#include <device.h>
#include <thread.h>

/**
 * Thread procedure for a thread that repeatedly reads TFTP packets from a UDP
 * device and provides them in a buffer to another thread.  This is a workaround
 * for the lack of timeout support in read().  Not intended to be used outside
 * of the TFTP code.
 *
 * @param udpdev
 *      UDP device to read from.
 * @param pkt
 *      Buffer to use for reading.
 * @param parent
 *      Thread to inform when a packet has been read.
 *
 * @return
 *      This thread never returns.
 */
thread tftpRecvPackets(int udpdev, struct tftpPkt *pkt, tid_typ parent)
{
    int result;

    for (;;)
    {
        /* Wait for parent thread to tell this thread to proceed.  */
        receive();

        /* Read the packet from the UDP device.  (Blocking.)  */
        result = read(udpdev, pkt, TFTP_MAX_PACKET_LEN);

        /* Send parent the length of the resulting packet (or SYSERR if an error
         * occurred).  */
        send(parent, result);
    }

    /* This thread is killed; it does not return.  */
    return SYSERR;
}
