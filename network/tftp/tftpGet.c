/**
 * @file tftpGet.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <clock.h>
#include <device.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <tftp.h>
#include <thread.h>
#include <udp.h>

/* Stress testing--- randomly ignore this percent of valid received data
 * packets.  */
#define TFTP_DROP_PACKET_PERCENT 0

/**
 * @ingroup tftp
 *
 * Download a file from a remote server using TFTP and passes its contents,
 * block-by-block, to a callback function.  This callback function can do
 * whatever it wants with the file data, such as store it all into a buffer or
 * write it to persistent storage.
 *
 * @param[in] filename
 *      Name of the file to download.
 * @param[in] local_ip
 *      Local protocol address to use for the connection.
 * @param[in] server_ip
 *      Remote protocol address to use for the connection (address of TFTP
 *      server).
 * @param[in] recvDataFunc
 *      Callback function that will be passed the file data block-by-block.  For
 *      each call of the callback function, the @p data (first) argument will be
 *      set to a pointer to the next block of data and the @p len (second)
 *      argument will be set to the block's length.  All data blocks will be the
 *      same size, except possibly the last, which can be anywhere from 0 bytes
 *      up to the size of the previous block(s) if any.
 *      <br/>
 *      In the current implementation, the block size (other than possibly for
 *      the last block) is fixed at 512 bytes.  However, implementations of this
 *      callback SHOULD handle larger block sizes since tftpGet() could be
 *      extended to support TFTP block size negotiation.
 *      <br/>
 *      This callback is expected to return ::OK if successful.  If it does not
 *      return ::OK, the TFTP transfer is aborted and tftpGet() returns this
 *      value.
 * @param[in] recvDataCtx
 *      Extra parameter that will be passed literally to @p recvDataFunc.
 *
 * @return
 *      ::OK on success; ::SYSERR if the TFTP transfer times out or fails, or if
 *      one of several other errors occur; or the value returned by @p
 *      recvDataFunc, if it was not ::OK.
 */
syscall tftpGet(const char *filename, const struct netaddr *local_ip,
                const struct netaddr *server_ip, tftpRecvDataFunc recvDataFunc,
                void *recvDataCtx)
{
    int udpdev;
    int udpdev2;
    int send_udpdev;
    int recv_udpdev;
    int retval;
    tid_typ recv_tid;
    uint num_rreqs_sent;
    uint block_recv_tries;
    uint next_block_number;
    ushort localpt;
    uint block_max_end_time = 0;  /* This value is not used, but
                                     gcc fails to detect it.  */
    uint block_attempt_time;
    struct tftpPkt pkt;

    /* Make sure the required parameters have been specified.  */
    if (NULL == filename || NULL == local_ip ||
        NULL == server_ip || NULL == recvDataFunc)
    {
        TFTP_TRACE("Invalid parameter.");
        return SYSERR;
    }

#ifdef ENABLE_TFTP_TRACE
    {
        char str_local_ip[20];
        char str_server_ip[20];
        netaddrsprintf(str_local_ip, local_ip);
        netaddrsprintf(str_server_ip, server_ip);
        TFTP_TRACE("Downloading %s from %s (using local_ip=%s)",
                   filename, str_server_ip, str_local_ip);
    }
#endif

    /* Allocate and open a UDP device (socket) to communicate with the TFTP
     * server on.  The local and remote protocol addresses are specified as
     * required parameters to this function.  The local port, which corresponds
     * to the client's TFTP Transfer Identifier (TID) as per RFC 1350, must be
     * allocated randomly; the UDP code handles this if 0 is passed as the local
     * port.  The remote port is always initially the well-known TFTP port (69),
     * but after receiving the first data packet it must be changed to the port
     * on which the server actually responded.
     *
     * However... the last point about the server responding on a different port
     * (which is unavoidable; it's how TFTP is designed) complicates things
     * significantly.  This is because the UDP code will *not* route the
     * server's response to the initial UDP device, as this device will be bound
     * to port 69, not the actual port the server responded on.  To work around
     * this problem without manually dealing with UDP headers, we create a
     * *second* UDP device, which initially listens on the port from which the
     * client sends the initial RRQ, but is initially *not* bound to any remote
     * port or address.  We then set UDP_FLAG_BINDFIRST on this second UDP
     * device so that the remote port and address are automatically filled in
     * when the response from the server is received.  Further packets sent from
     * the server are then received on this second UDP device, while further
     * packets sent from the client are then sent over this second UDP device
     * rather than the first since the second has the remote port correctly set.
     * */

    udpdev = udpAlloc();

    if (SYSERR == udpdev)
    {
        TFTP_TRACE("Failed to allocate first UDP device.");
        return SYSERR;
    }

    if (SYSERR == open(udpdev, local_ip, server_ip, 0, UDP_PORT_TFTP))
    {
        TFTP_TRACE("Failed to open first UDP device.");
        udptab[udpdev - UDP0].state = UDP_FREE;
        return SYSERR;
    }

    localpt = udptab[udpdev - UDP0].localpt;

    udpdev2 = udpAlloc();
    if (SYSERR == udpdev2)
    {
        TFTP_TRACE("Failed to allocate second UDP device.");
        retval = SYSERR;
        goto out_close_udpdev;
    }

    if (SYSERR == open(udpdev2, local_ip, NULL, localpt, 0))
    {
        TFTP_TRACE("Failed to open second UDP device.");
        retval = SYSERR;
        udptab[udpdev2 - UDP0].state = UDP_FREE;
        goto out_close_udpdev;
    }

    send_udpdev = udpdev;
    recv_udpdev = udpdev2;

    /* See lengthy comment above for explanation of this flag.  */
    control(recv_udpdev, UDP_CTRL_SETFLAG, UDP_FLAG_BINDFIRST, 0);

    TFTP_TRACE("Using UDP%d (for initial send) "
               "and UDP%d (for binding reply), client port %u",
               send_udpdev - UDP0, recv_udpdev - UDP0, localpt);

    /* Create receive thread.  This is a workaround to avoid having the
     * currently executing thread call read() on the UDP device, which can block
     * indefinitely.  */
    recv_tid = create(tftpRecvPackets, TFTP_RECV_THR_STK,
                      TFTP_RECV_THR_PRIO, "tftpRecvPackets", 3,
                      recv_udpdev, &pkt, gettid());
    if (isbadtid(recv_tid))
    {
        TFTP_TRACE("Failed to create TFTP receive thread.");
        retval = SYSERR;
        goto out_close_udpdev2;
    }
    ready(recv_tid, RESCHED_NO);

    /* Begin the download by requesting the file.  */
    retval = tftpSendRRQ(send_udpdev, filename);
    if (SYSERR == retval)
    {
        retval = SYSERR;
        goto out_kill_recv_thread;
    }
    num_rreqs_sent = 1;
    next_block_number = 1;

    /* Loop until file is fully downloaded or an error condition occurs.  The
     * basic idea is that the client receives DATA packets one-by-one, each of
     * which corresponds to the next block of file data, and the client ACK's
     * each one before the server sends the next.  But the actual code below is
     * a bit more complicated as it must handle timeouts, retries, invalid
     * packets, etc.  */
    block_recv_tries = 0;
    for (;;)
    {
        ushort opcode;
        ushort recv_block_number;
        struct netaddr *remote_address;
        bool wrong_source;
        ushort block_nbytes;

        /* Handle bookkeeping for timing out.  */

        block_attempt_time = clktime;
        if (block_recv_tries == 0)
        {
            uint timeout_secs;

            if (next_block_number == 1)
            {
                timeout_secs = TFTP_INIT_BLOCK_TIMEOUT;
            }
            else
            {
                timeout_secs = TFTP_BLOCK_TIMEOUT;
            }
            block_max_end_time = block_attempt_time + timeout_secs;
        }

        if (block_attempt_time <= block_max_end_time)
        {
            /* Try to receive the block using the appropriate timeout.  The
             * actual receive is done by another thread, executing
             * tftpRecvPacket(s).  */
            TFTP_TRACE("Waiting for block %u", next_block_number);
            block_recv_tries++;
            send(recv_tid, 0);
            retval = recvtime(1000 * (block_max_end_time -
                                      block_attempt_time) + 500);
        }
        else
        {
            /* Timeout was reached.  */
            retval = TIMEOUT;
        }

        /* Handle timeout.  */
        if (TIMEOUT == retval)
        {
            TFTP_TRACE("Receive timed out.");

            /* If the client is still waiting for the very first reply from the
             * server, don't fail on the first timeout; instead wait until the
             * client has had the chance to re-send the RRQ a few times.  */
            if (next_block_number == 1 &&
                num_rreqs_sent < TFTP_INIT_BLOCK_MAX_RETRIES)
            {
                TFTP_TRACE("Trying RRQ again (try %u of %u)",
                           num_rreqs_sent + 1, TFTP_INIT_BLOCK_MAX_RETRIES);
                retval = tftpSendRRQ(send_udpdev, filename);
                if (SYSERR == retval)
                {
                    break;
                }
                block_recv_tries = 0;
                num_rreqs_sent++;
                continue;
            }

            /* Timed out for real; clean up and return failure status.  */
            retval = SYSERR;
            break;
        }

        /* Return failure status if packet was not otherwise successfully
         * received for some reason.  */
        if (SYSERR == retval)
        {
            TFTP_TRACE("UDP device or message passing error; aborting.");
            break;
        }

        /* Otherwise, 'retval' is the length of the received TFTP packet.  */

        /* Begin extracting information from and validating the received packet.
         * What we're looking for is a well-formed TFTP DATA packet from the
         * correct IP address, where the block number is either that of the next
         * block or that of the previous block.  The very first block needs some
         * special handling, however; in particular, there is no previous block
         * in that case, and the remote network address needs to be checked to
         * verify the socket was actually bound to the server's network address
         * as expected.
         */
        remote_address = &udptab[recv_udpdev - UDP0].remoteip;
        opcode = net2hs(pkt.opcode);
        recv_block_number = net2hs(pkt.DATA.block_number);
        wrong_source = !netaddrequal(server_ip, remote_address);

        if (wrong_source || retval < 4 || TFTP_OPCODE_DATA != opcode ||
            (recv_block_number != (ushort)next_block_number &&
             (next_block_number == 1 ||
              recv_block_number != (ushort)next_block_number - 1)))
        {
            /* Check for TFTP ERROR packet  */
            if (!wrong_source && (retval >= 2 && TFTP_OPCODE_ERROR == opcode))
            {
                TFTP_TRACE("Received TFTP ERROR opcode packet; aborting.");
                retval = SYSERR;
                break;
            }
            TFTP_TRACE("Received invalid or unexpected packet.");

            /* If we're still waiting for the first valid reply from the server
             * but the bound connection is *not* from the server, reset the
             * BINDFIRST flag.  */
            if (wrong_source && next_block_number == 1)
            {
                irqmask im;
                TFTP_TRACE("Received packet is from wrong source; "
                           "re-setting bind flag.");
                im = disable();
                control(recv_udpdev, UDP_CTRL_BIND, 0, (long)NULL);
                control(recv_udpdev, UDP_CTRL_SETFLAG, UDP_FLAG_BINDFIRST, 0);
                restore(im);
            }

            /* Ignore the bad packet and try receiving again.  */
            continue;
        }

        /* Received packet is a valid TFTP DATA packet for either the next block
         * or the previous block.  */


    #if TFTP_DROP_PACKET_PERCENT != 0
        /* Stress testing.  */
        if (rand() % 100 < TFTP_DROP_PACKET_PERCENT)
        {
            TFTP_TRACE("WARNING: Ignoring valid TFTP packet for test purposes.");
            continue;
        }
    #endif

        /* If this is the first response from the server, set the actual port
         * that it responded on.  */
        if (next_block_number == 1)
        {
            send_udpdev = recv_udpdev;
            TFTP_TRACE("Server responded on port %u; bound socket",
                       udptab[recv_udpdev - UDP0].remotept);
        }

        /* Handle receiving the next data block.  */
        block_nbytes = TFTP_BLOCK_SIZE;
        if (recv_block_number == (ushort)next_block_number)
        {
            block_nbytes = retval - 4;
            TFTP_TRACE("Received block %u (%u bytes)",
                       recv_block_number, block_nbytes);

            /* Feed received data into the callback function.  */
            retval = (*recvDataFunc)(pkt.DATA.data, block_nbytes,
                                     recvDataCtx);
            /* Return if callback did not return OK.  */
            if (OK != retval)
            {
                break;
            }
            next_block_number++;
            block_recv_tries = 0;
        }

        /* Acknowledge the block received.  */
        retval = tftpSendACK(send_udpdev, recv_block_number);

        /* A TFTP Get transfer is complete when a short data block has been
         * received.   Note that it doesn't really matter from the client's
         * perspective whether the last data block is acknowledged or not;
         * however, the server would like to know so it doesn't keep re-sending
         * the last block.  For this reason we did send the final ACK packet but
         * will ignore failure to send it.  */
        if (block_nbytes < TFTP_BLOCK_SIZE)
        {
            retval = OK;
            break;
        }

        /* Break if sending the ACK failed.  */
        if (SYSERR == retval)
        {
            break;
        }
    }
    /* Clean up and return.  */
out_kill_recv_thread:
    kill(recv_tid);
out_close_udpdev2:
    close(udpdev2);
out_close_udpdev:
    close(udpdev);
    return retval;
}
