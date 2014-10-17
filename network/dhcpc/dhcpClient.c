/**
 * @file     dhcpClient.c
 */
/* Embedded Xinu, Copyright (C) 2008, 2013.  All rights reserved. */

#include <clock.h>
#include <device.h>
#include <ether.h>
#include "dhcp.h"
#include <stdlib.h>
#include <string.h>
#include <thread.h>

/**
 * @ingroup dhcpc
 *
 * DHCP (Dynamic Host Configuration Protocol) client for IPv4 autoconfiguration.
 *
 * TODO: This function returns immediately once the IP address has been
 * assigned.  It does not handle renewing leases.
 *
 * @param[in] descrp
 *      Device descriptor for the network device on which to open the DHCP
 *      client on.  This must be a network device that is open but has no
 *      network interface brought up on it.
 * @param[in] timeout
 *      Seconds to wait before timing out (must be a positive integer).
 * @param[out] data
 *      On success, this structure is filled in with the information about the
 *      IPv4 configuration.  At least the @ref dhcpData::ip "ip" and @ref
 *      dhcpData::mask "mask" members will be valid.  The @ref dhcpData::gateway
 *      "gateway", @ref dhcpData::bootfile "bootfile", and @ref
 *      dhcpData::next_server "next_server" members are optional and will be
 *      left as all 0's if not provided.
 *
 * @return
 *      ::OK if successful; ::SYSERR if sending a DHCPDISCOVER packet failed or
 *      if a parameter was invalid; ::TIMEOUT if timed out.
 */
syscall dhcpClient(int descrp, uint timeout, struct dhcpData *data)
{
    int retval;
    uint recvTimeout = 2000;  /* Milliseconds to wait before timing out a
                                 given receive (not the whole client)  */
    uint delay = 1000;  /* Milliseconds to wait after non-timeout error */
    ulong starttime = clktime;

    /* Check for invalid parameters.  */
#if NETHER
    if (descrp < ETH0 || descrp >= ETH0 + NETHER)
#endif
    {
        DHCP_TRACE("Bad device descriptor.\n");
        return SYSERR;
    }

    if (0 == timeout)
    {
        DHCP_TRACE("Timeout cannot be zero.\n");
        return SYSERR;
    }

    if (NULL == data)
    {
        DHCP_TRACE("No data buffer provided.\n");
        return SYSERR;
    }

    if (NULL != netLookup(descrp))
    {
        DHCP_TRACE("Network interface is up on device.\n");
        return SYSERR;
    }

    bzero(data, sizeof(*data));
    data->state = DHCPC_STATE_INIT;

    while (clktime <= starttime + timeout)
    {
        switch (data->state)
        {
        case DHCPC_STATE_INIT:
            /* Initialize transfer data.  */

            data->cxid = rand();          /* Choose random context ID */
            data->starttime = clktime;    /* Set starting time   */

            data->clientIpv4Addr = 0;     /* Client IP address is unknown  */
            data->serverIpv4Addr = 0;     /* Server IP address is unknown  */

            /* Client hardware address is known; get it from network device.  */
            if (SYSERR == control(descrp, ETH_CTRL_GET_MAC,
                                  (long)data->clientHwAddr, 0))
            {
                DHCP_TRACE("Failed to get client hardware address");
                return SYSERR;
            }

            /* Server hardware address is unknown  */
            bzero(data->serverHwAddr, ETH_ADDR_LEN);

            /* Broadcast DHCPDISCOVER.  */
            DHCP_TRACE("Sending DHCPDISCOVER");
            retval = dhcpSendRequest(descrp, data);

            if (OK == retval)
            {
                DHCP_TRACE("Sent DHCPDISCOVER");
                data->state = DHCPC_STATE_SELECTING;
            }
            else
            {
                DHCP_TRACE("Failed to send DHCPDISCOVER; returning failure");
                return SYSERR;
            }
            break;

        case DHCPC_STATE_SELECTING:
            /* Wait for DHCPOFFER from any server.  */
            DHCP_TRACE("Waiting for DHCPOFFER");
            retval = dhcpRecvReply(descrp, data, recvTimeout);

            if (OK != retval)
            {
                DHCP_TRACE("Failed to receive DHCPOFFER");
                data->state = DHCPC_STATE_INIT;
                if (TIMEOUT != retval)
                {
                    sleep(delay);
                }
                break;
            }

            /* Send DHCPREQUEST to the server based on the DHCPOFFER.  */
            DHCP_TRACE("Sending DHCPREQUEST");
            retval = dhcpSendRequest(descrp, data);

            if (OK == retval)
            {
                DHCP_TRACE("Sent DHCPREQUEST");
                data->state = DHCPC_STATE_REQUESTING;
            }
            else
            {
                DHCP_TRACE("Failed to send DHCPREQUEST");
                data->state = DHCPC_STATE_INIT;
                sleep(delay);
            }
            break;

        case DHCPC_STATE_REQUESTING:
            /* Wait for DHCPACK from the server to whom we sent the DHCPREQUEST.
             * */
            DHCP_TRACE("Waiting for DHCPACK");
            retval = dhcpRecvReply(descrp, data, recvTimeout);

            if (OK == retval)
            {
                DHCP_TRACE("Received DHCPACK");
                data->state = DHCPC_STATE_BOUND;
                return OK;
            }
            else
            {
                DHCP_TRACE("Failed to receive DHCPACK");
                data->state = DHCPC_STATE_INIT;
                if (TIMEOUT != retval)
                {
                    sleep(delay);
                }
            }
            break;
        }
    }
    return TIMEOUT;
}
