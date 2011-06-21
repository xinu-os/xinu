/**
 * @file     xsh_telnet.c
 * @provides xsh_telnet
 *
 * $Id: xsh_telnet.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <interrupt.h>
#include <shell.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ipv4.h>
#include <tcp.h>
#include <thread.h>
#include <telnet.h>
#include <tty.h>
#include <ether.h>

static void usage(char *command);
thread telnetRecv(ushort);
thread telnetSend(ushort);
static int echoNegotiation(int, int, ushort);
static void sendOption(ushort, uchar, uchar);

/**
 * Shell command (telnet).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return 0 for success, 1 for error
 */
shellcmd xsh_telnet(int nargs, char *args[])
{
    int i;
    ushort port = 0;
    struct netaddr host;
    struct netaddr *localhost;
    struct netif *interface;

    tid_typ recvthr;
    tid_typ sendthr;
    int msg = 0;
    int dev;

    /* Enable interrupts */
    enable();

    for (i = 1; i < nargs; i++)
    {
        /* Help */
        if (0 == strncmp(args[i], "--help", 7))
        {
            usage(args[0]);
            return 0;
        }

        /* Host */
        if (0 == host.len)
        {
            if (SYSERR == dot2ipv4(args[i], &host))
            {
                fprintf(stderr, "Invalid hostname\n");
                return 1;
            }
            continue;
        }

        /* Port */
        if (0 == port)
        {
            port = atoi(args[i]);
            if (port == 0)
            {
                fprintf(stderr, "Port must be greater than 0\n");
                return 1;
            }
            continue;
        }

        /* Otherwise */
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    /* Verify a valid number of arguments were processed */
    if (0 == host.len)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    /* Set default port if none is specified */
    if (port == 0)
    {
        port = TELNET_PORT;
    }

    /* Print that the client is trying to connect to the server */
    printf("Trying %s...\n", args[1]);

    interface = netLookup((ethertab[0].dev)->num);
    if (NULL == interface)
    {
        fprintf(stderr, "No network interface found\n");
        return 1;
    }
    localhost = &(interface->ip);

    /* Open connection */
#if NTCP
    dev = tcpAlloc();
#else
    dev = SYSERR;
#endif                          /* NTCP */
    if (SYSERR == dev)
    {
        fprintf(stderr, "Failed to allocate TCP device\n");
        return 1;
    }


    if (SYSERR == open(dev, localhost, &host, NULL, port, TCP_ACTIVE))
    {
        fprintf(stderr, "Failed to establish connection\n");
        return 1;
    }

    /* Display a connection prompt and tell the user how to quit */
    printf("Connected to %s.\nQuit character is '^]'.\n", args[1]);

    recvthr =
        create(telnetRecv, SHELL_CMDSTK, SHELL_CMDPRIO, "telnet_recv", 1,
               dev);
    sendthr =
        create(telnetSend, SHELL_CMDSTK, SHELL_CMDPRIO, "telnet_send", 1,
               dev);

    if ((SYSERR == recvthr) || (SYSERR == sendthr))
    {
        kill(recvthr);
        kill(sendthr);
        close(dev);
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
    control(stdin, TTY_CTRL_SET_IFLAG, TTY_ECHO, NULL);
    control(stdin, TTY_CTRL_CLR_IFLAG, TTY_IRAW, NULL);
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

    control(stdin, TTY_CTRL_SET_IFLAG, TTY_ECHO, NULL);
    control(stdin, TTY_CTRL_CLR_IFLAG, TTY_IRAW, NULL);

    return 0;
}

static void usage(char *command)
{
    printf("Usage:\n");
    printf("\t%s <HOSTNAME> <PORT>\n", command);
    printf("Description:\n");
    printf("\tprovides an interface to remote consoles.\n");
    printf("Options:\n");
    printf
        ("\t<HOSTNAME>\tname or IP address of the host to connect to\n");
    printf("\t<PORT>\t\tport on which to connect. Default is 23 if no\n");
    printf("\t\t\tport is specified\n");
    printf("\t--help\t\tdisplay this help and exit\n");
}

thread telnetRecv(ushort dev)
{
    int ch;
    bool sgaRemoteEnabled = FALSE;
    bool sgaLocalEnabled = FALSE;
    int echoState = TELNET_ECHO_NO_ECHO;

    enable();

    /* read until connection closed */
    while (read(dev, &ch, 1) != SYSERR)
    {
        switch (ch)
        {
        case TELNET_IAC:
            switch (getc(dev))
            {
            case TELNET_WILL:
                /* get single raw characters if server agrees to echo option */
                if (((ch = getc(dev)) == TELNET_ECHO) &&
                    (echoState == TELNET_ECHO_NO_ECHO))
                {
                    sendOption(dev, TELNET_DO, TELNET_ECHO);
                    echoState = TELNET_ECHO_OTHER_ECHOES;
                    /* set local TTY to only print remotely echoed chars */
                    control(stdin, TTY_CTRL_CLR_IFLAG, TTY_ECHO, NULL);
                    control(stdin, TTY_CTRL_SET_IFLAG, TTY_IRAW, NULL);
                }
                else if ((ch == TELNET_SUPPRESS_GA)
                         && !(sgaRemoteEnabled))
                {
                    sendOption(dev, TELNET_DO, TELNET_SUPPRESS_GA);
                    sgaRemoteEnabled = TRUE;
                }
                else
                {
                    sendOption(dev, TELNET_DONT, ch);
                }

                break;
            case TELNET_DO:
                if ((ch = getc(dev)) == SYSERR)
                {
                    printf("\nConnection closed by foreign host.\n");
                    return OK;
                }
                else if (ch == TELNET_SUPPRESS_GA && !(sgaLocalEnabled))
                {
                    sendOption(dev, TELNET_WILL, TELNET_SUPPRESS_GA);
                    sgaLocalEnabled = TRUE;
                }
                else
                {
                    sendOption(dev, TELNET_WONT, ch);
                }
                break;
            case TELNET_WONT:
                if (SYSERR == (ch = getc(dev)))
                {
                    printf("\nConnection closed by foreign host.\n");
                    return OK;
                }
                else if (TELNET_ECHO == ch)
                {
                    echoState =
                        echoNegotiation(echoState, TELNET_DONT, dev);

                    /* set local TTY to local echo */
                    if (echoState == TELNET_ECHO_NO_ECHO)
                    {
                        control(stdin, TTY_CTRL_SET_IFLAG, TTY_ECHO,
                                NULL);
                        control(stdin, TTY_CTRL_CLR_IFLAG, TTY_IRAW,
                                NULL);
                    }
                }
                else if ((TELNET_SUPPRESS_GA == ch) && sgaRemoteEnabled)
                {
                    sendOption(dev, TELNET_DONT, TELNET_SUPPRESS_GA);
                    sgaRemoteEnabled = FALSE;
                }
                break;
            case TELNET_DONT:
                if (SYSERR == (ch = getc(dev)))
                {
                    printf("\nConnection closed by foreign host.\n");
                    return OK;
                }
                else if (TELNET_ECHO == ch)
                {
                    echoState =
                        echoNegotiation(echoState, TELNET_DONT, dev);
                }
                else if ((TELNET_SUPPRESS_GA == ch) && sgaLocalEnabled)
                {
                    sendOption(dev, TELNET_WONT, TELNET_SUPPRESS_GA);
                    sgaLocalEnabled = FALSE;
                }
                break;
            case TELNET_IAC:
                /* escaped IAC sequence, so print a single IAC char */
                printf("%c", TELNET_IAC);
                break;
            case TELNET_AYT:
                /* reply to server with useless data so it doesn't print */
                putc(dev, '\0');
                break;
            case TELNET_EC:
                /* delete last char */
                printf("%c", '\b');
                break;
            case SYSERR:
                printf("\nConnection closed by foreign host.\n");
                return OK;
            default:
                break;
            }

            break;
        default:
            printf("%c", ch);
            break;

        }
    }

    printf("\nConnection closed by foreign host.\n");
    return OK;
}

thread telnetSend(ushort dev)
{
    char ch;

    enable();

    while (SYSERR != (ch = getc(stdin)))
    {
        /* close connection on CTRL + ] */
        if (ch == TELNET_CHR_CLOSE)
        {
            printf("\nConnection closed by user.\n");
            return OK;
        }

        /* Send Erase Character command instead of backspace */
        if ((ch == '\b') || (ch == TELNET_CHR_DEL))
        {
            sendOption(dev, TELNET_EC, '\0');
            continue;
        }

        /* Send Erase Line command instead of CTRL + U */
        if (ch == TELNET_CHR_EL)
        {
            sendOption(dev, TELNET_EL, '\0');
            continue;
        }

        if (ch == '\n')
        {
            putc(dev, '\r');
        }

        if (SYSERR == putc(dev, ch))
        {
            printf("\nConnection closed by foreign host.\n");
            return OK;
        }
    }

    return SYSERR;
}

/* provides state machine for echo negotiation 
 * @returns new state of echo 
 */
static int echoNegotiation(int currentState, int command,
                           ushort writeDevice)
{
    switch (currentState)
    {
    case TELNET_ECHO_OTHER_ECHOES:
        if (TELNET_WONT == command)
        {
            sendOption(writeDevice, TELNET_DONT, TELNET_ECHO);
            return TELNET_ECHO_NO_ECHO;
        }
        else if (TELNET_DO == command)
        {
            sendOption(writeDevice, TELNET_WONT, TELNET_ECHO);
            return TELNET_ECHO_OTHER_ECHOES;
        }
        break;
    case TELNET_ECHO_NO_ECHO:
        if (TELNET_DO == command)
        {
            sendOption(writeDevice, TELNET_WONT, TELNET_ECHO);
            return TELNET_ECHO_NO_ECHO;
        }
        else if (TELNET_WILL == command)
        {
            sendOption(writeDevice, TELNET_DO, TELNET_ECHO);
            return TELNET_ECHO_OTHER_ECHOES;
        }
        break;
    default:
        // Error, unknown state
        break;
    }
    return currentState;
}

/* sends a telnet formatted option to a server */
static void sendOption(ushort dev, uchar command, uchar option)
{
    uchar cmdbuf[3];
    cmdbuf[0] = TELNET_IAC;
    cmdbuf[1] = command;
    cmdbuf[2] = option;
    write(dev, (void *)cmdbuf, 3);
}
