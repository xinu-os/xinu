/**
 * @file httpServer.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>

#include <http.h>
#include <network.h>
#include <shell.h>
#include <tcp.h>
#include <thread.h>


thread killHttpServer(uint, tid_typ, uint);
thread httpServer(int, int);

/**
 * HTTP server kick start thread
 * @return OK or SYSERR
 */
thread httpServerKickStart(int netDescrp)
{
    int tcpdev;
    char thrname[TNMLEN];
    int tid;
    int cursem;

    cursem = activeXWeb;
    /* Only one active web server at a time */
    wait(activeXWeb);
    /* Make sure activeXWeb semaphore was not reset */
    if (cursem != activeXWeb)
    {
        return SYSERR;
    }

    /* Allocate TCP device */
    tcpdev = tcpAlloc();
    if (isbadtcp(tcpdev))
    {
        fprintf(stderr, "No TCP devices available for initialization.\n");
        return SYSERR;
    }

    sprintf(thrname, "XWeb_%d\0", (devtab[tcpdev].minor));
    tid = create((void *)httpServer, INITSTK, INITPRIO, thrname,
                 2, netDescrp, tcpdev);
    ready(tid, RESCHED_NO);

    return tid;
}

/**
 * HTTP server thread
 * @param host IP address of interface on which to listen
 * @param gentcpdev the allocated tcp device for general listening
 * @return OK or SYSERR
 */
thread httpServer(int netDescrp, int gentcpdev)
{
    tid_typ shelltid, killtid;
    int tcpdev, httpdev;
    char thrname[TNMLEN];
    bool noGenHTTP;
    struct netaddr *host;
    struct netif *nif;

    noGenHTTP = FALSE;
    wait(maxhttp);              /* Make sure max HTTP threads not reached */

    /* Allocate HTTP device */
    httpdev = httpAlloc();

    /* Received bad http device, close out allocated resources */
    if (isbadhttp(httpdev))
    {
        printf("failed to allocate proper HTTP device\n");
        signal(maxhttp);
        return SYSERR;
    }

    /* Back up the general tcpdev number */
    tcpdev = gentcpdev;

    /* Look up the network descriptor */
    nif = netLookup(netDescrp);
    if (SYSERR == (int)nif)
    {
        fprintf(stderr, "%s is not associated with an active network",
                devtab[netDescrp].name);
        fprintf(stderr, " interface.\n");
        return SYSERR;
    }
    host = &(nif->ip);

    /* Open HTTP device */
    if (SYSERR == (long)open(httpdev, tcpdev))
    {
        fprintf(stderr, "httpOpen SYSERR\n");
        close(tcpdev);
        close(httpdev);
        return SYSERR;
    }

    /* Create web shell */
    sprintf(thrname, "XWebShell_%d\0", (devtab[tcpdev].minor));
    shelltid = create((void *)shell, INITSTK, INITPRIO, thrname, 3,
                      httpdev, httpdev, DEVNULL);
    if (isbadtid(shelltid))
    {
        fprintf(stderr, "shell creation bad tid\n");
        close(httpdev);
        close(tcpdev);
        return SYSERR;
    }

    /* Spawn thread that will wait for kill httpserver thread signal */
    sprintf(thrname, "XWebKillerD_%d\0", (devtab[tcpdev].minor));
    killtid = create((void *)killHttpServer, INITSTK, INITPRIO, thrname,
                     3, httpdev, shelltid, tcpdev);

    /* Ready spawned threads */
    ready(shelltid, RESCHED_NO);
    ready(killtid, RESCHED_NO);

    /* Open TCP device */
    if (SYSERR ==
        (long)open(tcpdev, host, NULL, HTTP_LOCAL_PORT, NULL,
                   TCP_PASSIVE))
    {
        int temp;
        sleep(TCP_TWOMSL + 500);
        if (SYSERR ==
            (temp = (long)open(tcpdev, host, NULL, HTTP_LOCAL_PORT, NULL,
                               TCP_PASSIVE)))
        {
            fprintf(CONSOLE, "open returns: %d\n", temp);
            fprintf(stderr, "tcpOpen SYSERR, devnum: %d\n", tcpdev);
            kill(shelltid);
            close(tcpdev);
            close(httpdev);
            return SYSERR;
        }
        fprintf(CONSOLE, "open returns: %d\n", temp);
    }

    /* Allocate TCP device */
    gentcpdev = tcpAlloc();
    if (isbadtcp(gentcpdev))
    {
        /* Wait for a free http device */
        wait(maxhttp);

        /* Acquire a TCP device through allocation */
        while ((ushort)SYSERR == (gentcpdev = tcpAlloc()))
        {
            /* Do nothing, yield processor */
            yield();
        }

        /* Everything acquired, spawn general http device listener */
        if (semcount(activeXWeb) <= 0)
        {
            sprintf(thrname, "XWeb_%d\0", (devtab[gentcpdev].minor));
            ready(create((void *)httpServer, INITSTK, INITPRIO,
                         thrname, 2, netDescrp, gentcpdev), RESCHED_NO);
        }

        /* Did not consume http device, just waited for availability */
        signal(maxhttp);
    }
    else
    {
        sprintf(thrname, "XWeb_%d\0", (devtab[gentcpdev].minor));
        ready(create((void *)httpServer, INITSTK, INITPRIO,
                     thrname, 2, netDescrp, gentcpdev), RESCHED_NO);
    }

    return OK;
}

/**
 * Kills HTTP server spawned threads and close devices
 * @param httpdev HTTP device to close
 * @param shelltid shell thread ID to kill
 * @param tcpdev TCP device to close
 * @param servertid server thread ID to kill
 * @return thread return status
 */
thread killHttpServer(uint httpdev, tid_typ shelltid, uint tcpdev)
{
    device *devptr;
    struct http *webptr;
    /* Acquire a pointer to the http device */
    devptr = (device *)&devtab[httpdev];
    if (NULL == devptr)
    {
        return SYSERR;
    }

    webptr = &httptab[devptr->minor];

    /* wait for the connection close flag */
    wait(webptr->closeall);

    /* kill the webshell */
    kill(shelltid);
    /* close tcp connection */
    close(tcpdev);
    /* close http device */
    close(httpdev);

    return OK;
}
