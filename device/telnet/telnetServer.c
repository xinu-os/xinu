/**
 * @file telnetServer.c
 * @provides telnetServer
 *
 * $Id: telnetServer.c 2116 2009-11-03 20:55:05Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <stddef.h>
#include <device.h>
#include <tcp.h>
#include <shell.h>
#include <thread.h>
#include <telnet.h>

thread telnetServerKiller(ushort, ushort);

/**
 * Start telnet server
 * @param ethdev  interface on which telnet server will listen
 * @param port  port on which to start the server
 * @param telnetdev  telnet device to use for connection
 * @param shellname     shell device to use for connection
 * @return      OK on success SYSERR on failure
 */
thread telnetServer(int ethdev, int port, ushort telnetdev,
                    char *shellname)
{
    tid_typ tid, killtid;
    ushort tcpdev;
    struct netif *interface;
    struct netaddr *host;
    char thrname[16];
    uchar buf[6];

    TELNET_TRACE("ethdev %d, port %d, telnet %d", ethdev, port,
                 telnetdev);
    /* find netaddr of ethernet device */
    interface = netLookup(ethdev);
    if (NULL == interface)
    {
        close(telnetdev);
        return SYSERR;
    }
    host = &(interface->ip);

    enable();

    while (TRUE)
    {
        tcpdev = tcpAlloc();
        if (SYSERR == (short)tcpdev)
        {
            close(telnetdev);
            fprintf(stderr,
                    "telnet server failed to allocate TCP device\n");
            return SYSERR;
        }
        sprintf(thrname, "telnetSvrKill_%d\0", (devtab[telnetdev].minor));
        killtid = create((void *)telnetServerKiller, INITSTK, INITPRIO,
                         thrname, 2, telnetdev, tcpdev);
        ready(killtid, RESCHED_YES);

        if (open(tcpdev, host, NULL, port, NULL, TCP_PASSIVE) < 0)
        {
            kill(killtid);
            close(tcpdev);
            close(telnetdev);
            fprintf(stderr,
                    "telnet server failed to open TCP socket %d\n",
                    tcpdev);
            return SYSERR;
        }

        if (SYSERR == open(telnetdev, tcpdev))
        {
            kill(killtid);
            close(tcpdev);
            close(telnetdev);
            fprintf(stderr,
                    "telnet server failed to open TELNET device %d\n",
                    telnetdev);
            return SYSERR;
        }
#ifdef TELNET0
        TELNET_TRACE("telnetServer() opened telnet %d on TCP%d\n",
                     telnetdev - TELNET0, tcpdev - TCP0);
#endif

        /* Request these options to the client */
        buf[0] = TELNET_IAC;
        buf[1] = TELNET_WILL;
        buf[2] = TELNET_ECHO;
        buf[3] = TELNET_IAC;
        buf[4] = TELNET_DO;
        buf[5] = TELNET_SUPPRESS_GA;
        write(tcpdev, (void *)buf, 6);

        TELNET_TRACE
            ("telnetServer() sending WILL ECHO and Suppress GA\n");

        // open shell on TELNET device
        tid = create((void *)shell, INITSTK, INITPRIO, shellname, 3,
                     telnetdev, telnetdev, telnetdev);
        if (SYSERR == tid)
        {
            close(tcpdev);
            close(telnetdev);
            kill(killtid);
            return SYSERR;
        }
        /* Clear any pending messages */
        recvclr();
        TELNET_TRACE("telnetServer() spawning shell thread %d\n", tid);
        ready(tid, RESCHED_YES);

        // loop until child process dies
        while (recvclr() != tid)
        {
            sleep(200);
            control(telnetdev, TELNET_CTRL_FLUSH, 0, 0);
        }

        if (SYSERR == close(tcpdev))
        {
            close(telnetdev);
            kill(killtid);
            return SYSERR;
        }
        if (SYSERR == close(telnetdev))
        {
            kill(killtid);
            return SYSERR;
        }
    }

    return SYSERR;
}

/**
 * Kills telnet server that was spawned 
 * @param telnetdev telnet device to close
 * @param tcpdev tcp device to close
 * @return thread return status
 */
thread telnetServerKiller(ushort telnetdev, ushort tcpdev)
{
    int minor, sem;

    enable();

    minor = devtab[telnetdev].minor;
    sem = telnettab[minor].killswitch;

    /* Wait on device close semaphore */
    wait(sem);

    TELNET_TRACE("Killing server");

    /* Close the tcp device */
    close(tcpdev);

    /* Close the telnet device */
    close(telnetdev);

    return OK;
}
