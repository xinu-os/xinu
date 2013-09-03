/**
 * @file     udpWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <udp.h>

/**
 * @ingroup udpexternal
 *
 * Write data to a UDP device, thereby sending it over the network in one or
 * more UDP packets using the address/port parameters with which the UDP device
 * is configured.
 *
 * Note: depending on the lower levels of the network stack, this function
 * likely only buffers the UDP packet(s) to be sent at some later time.
 * Therefore, they may not have actually been transmitted on the wire when this
 * function returns.
 *
 * The UDP device MUST be open and MUST remain open throughout the execution of
 * this function.
 *
 * @param devptr
 *      Device entry for the UDP device.
 * @param buf
 *      Buffer of data to be sent.  If the UDP device is in the default mode,
 *      this is interpreted as the UDP payload to send, which will be split up
 *      among multiple UDP packets if its size exceeds ::UDP_MAX_DATALEN bytes.
 *      Alternatively, if the UDP device is in @ref UDP_FLAG_PASSIVE "passive
 *      mode", the data is intepreted as a single UDP packet including the UDP
 *      pseudo-header, followed by the UDP header, followed by the UDP payload.
 * @param len
 *      Number of bytes of data to send (length of @p buf).
 *
 * @return
 *      If any packets were successfully sent, returns the number of bytes of
 *      data successfully sent, which may be less than @p len in the case of an
 *      error.  If the UDP device is not in passive mode and @p len was 0, no
 *      packets will be sent and 0 will be returned.  Otherwise, returns
 *      ::SYSERR.
 */
devcall udpWrite(device *devptr, const void *buf, uint len)
{
    struct udp *udpptr;
    int result;

    udpptr = &udptab[devptr->minor];

    if (udpptr->flags & UDP_FLAG_PASSIVE)
    {
        /* If passive UDP device, pass in pseudoheader + header + payload */
        if (len > sizeof(struct udpPseudoHdr) + UDP_HDR_LEN + UDP_MAX_DATALEN ||
            len < sizeof(struct udpPseudoHdr) + UDP_HDR_LEN)
        {
            UDP_TRACE("Invalid size (%d) passed to passive UDP device", len);
            return SYSERR;
        }
        result = udpSend(udpptr, len, buf);
        if (OK == result)
        {
            return len;
        }
        else
        {
            return SYSERR;
        }
    }
    else
    {
        uint pktsize;
        uint count;

        /* Check if we have a specified remote port and ip */
        if (0 == udpptr->remotept || 0 == udpptr->remoteip.type)
        {
            UDP_TRACE("No specified remote port or IP address.");
            return SYSERR;
        }

        /* Carry out the actual writing */
        for (count = 0; count < len; count += pktsize)
        {
            uint bytes_remaining = len - count;

            if (bytes_remaining >= UDP_MAX_DATALEN)
            {
                pktsize = UDP_MAX_DATALEN;
            }
            else
            {
                pktsize = bytes_remaining;
            }

            result = udpSend(udpptr, pktsize, (const uchar*)buf + count);
            if (OK != result)
            {
                if (0 == count)
                {
                    return SYSERR;
                }
                else
                {
                    return count;
                }
            }
        }
        return len;
    }
}
