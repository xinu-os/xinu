/**
 * @file     xsh_nc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <ctype.h>
#include <device.h>
#include <interrupt.h>
#include <ipv4.h>
#include <shell.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tcp.h>
#include <thread.h>
#include <udp.h>

#if NETHER
static void usage(char *command);
thread ncRecv(ushort);
thread ncSend(ushort);

int argError(char *arg)
{
    fprintf(stderr, "Invalid argument '%s', try nc --help\n", arg);
    return 1;
}

int connError(void)
{
    fprintf(stderr, "Failed to establish a connection\n");
    return 1;
}

/**
 * @ingroup shell
 *
 * Shell command (nc).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return 0 for success, 1 for error
 */
shellcmd xsh_nc(int nargs, char *args[])
{
    int a;
    ushort port;
    bool listen = FALSE;
    struct netaddr dst;
    struct netaddr src;
    tid_typ recvthr;
    tid_typ sendthr;
    int msg = 0;
    int dev;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && 0 == strncmp(args[1], "--help", 7))
    {
        usage(args[0]);
        return 0;
    }

    port = 0;
    dst.type = NULL;
    src.type = NULL;

    /* Parse arguments */
    for (a = 1; a < nargs; a++)
    {
        if (args[a][0] != '-')
        {
            if (!listen && NULL == dst.type)
            {
                if (!isdigit(args[a][0]))
                {
                    return argError(args[a]);
                }
                dot2ipv4(args[a], &dst);
            }
            else if (listen && NULL == src.type)
            {
                if (!isdigit(args[a][0]))
                {
                    return argError(args[a]);
                }
                dot2ipv4(args[a], &src);
            }
            else if (NULL == port)
            {
                if (!isdigit(args[a][0]))
                {
                    return argError(args[a]);
                }
                port = atoi(args[a]);
            }
        }
        else
        {
            switch (args[a][1])
            {
                /* Listen on port */
            case 'l':
                listen = TRUE;
                break;
                /* Source IP */
            case 's':
                a++;
                if (a >= nargs || !isdigit(args[a][0]))
                {
                    return argError(args[a - 1]);
                }
                dot2ipv4(args[a], &src);
                break;
            default:
                return argError(args[a]);
            }
        }
    }

    /* Verify arguments */
    if (NULL == src.type)
    {
        fprintf(stderr, "Source/listen IP address required\n");
        return 1;
    }
    if (NULL == port)
    {
        fprintf(stderr, "Invalid port\n");
        return 1;
    }
    if (!listen && NULL == dst.type)
    {
        fprintf(stderr, "Remote IP address required\n");
        return 1;
    }

    /* Allocate a TCP device */
#ifdef NTCP
    dev = tcpAlloc();
#else
    dev = SYSERR;
#endif                          /* NTCP */
    if (SYSERR == dev)
    {
        return connError();
    }

    if (listen)
    {
        if (SYSERR == open(dev, &src, NULL, port, NULL, TCP_PASSIVE))
        {
            close(dev);
            return connError();
        }
    }
    else
    {
        if (SYSERR == open(dev, &src, &dst, NULL, port, TCP_ACTIVE))
        {
            close(dev);
            return connError();
        }
    }

    recvthr =
        create(ncRecv, SHELL_CMDSTK, SHELL_CMDPRIO, "nc_recv", 1, dev);
    sendthr =
        create(ncSend, SHELL_CMDSTK, SHELL_CMDPRIO, "nc_send", 1, dev);
    if ((SYSERR == recvthr) || (SYSERR == sendthr))
    {
        kill(recvthr);
        kill(sendthr);
        close(dev);
        fprintf(stderr, "Failed to spawn threads");
        return 1;
    }
    thrtab[recvthr].fdesc[0] = stdin;
    thrtab[recvthr].fdesc[1] = stdout;
    thrtab[recvthr].fdesc[2] = stderr;
    thrtab[sendthr].fdesc[0] = stdin;
    thrtab[sendthr].fdesc[1] = stdout;
    thrtab[sendthr].fdesc[2] = stderr;

    /* Start both threads */
    while (recvclr() != NOMSG);
    ready(recvthr, RESCHED_YES);
    ready(sendthr, RESCHED_NO);

    /* Wait for one thread to die */
    while ((msg != recvthr) && (msg != sendthr))
    {
        msg = receive();
    }
    sleep(10);

    /* Kill both threads */
    kill(recvthr);
    kill(sendthr);

    close(dev);
    return 0;
}

static void usage(char *command)
{
    printf("Usage:\n");
    printf("\t%s -s <LOCAL_IP> <REMOTE_IP> <PORT>\n", command);
    printf("\t%s -l <LISTEN_IP> <PORT>\n", command);
    printf("Description:\n");
    printf
        ("\tNetcat sends and receives data across the network using TCP\n");
    printf("Options:\n");
    printf("\t<LOCAL_IP>\tIP address of interface to send packets on\n");
    printf("\t<REMOTE_IP>\tIP address of the host to connect to\n");
    printf("\t<PORT>\t\tport on which to connect or listen\n");
    printf("\t<LISTEN_IP>\tlisten on the interface with IP listen_ip\n");
    printf("\t-l\t\tlisten for an incoming connection\n");
    printf
        ("\t-s\t\tpackets are sent on the interface with IP local_ip\n");
    printf("\t--help\t\tdisplay this help and exit\n");
}

thread ncRecv(ushort dev)
{
    char ch;
    uint bytes = 0;
    int result;

    result = read(dev, &ch, 1);
    while (result >= 0)
    {
        bytes++;
        printf("%c", ch);
        result = read(dev, &ch, 1);
    }

    return OK;
}

thread ncSend(ushort dev)
{
    char buf[1000];
    int count = 0;

    count = read(stdin, buf, 1000);
    while (count != EOF)
    {
        if (write(dev, buf, count) == SYSERR)
        {
            return OK;
        }
        count = read(stdin, buf, 1000);
    }
    return OK;
}
#endif /* NETHER */
