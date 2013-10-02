TCP Echo Test
=============

About
-----

This is a simple TCP usage example. It is an echo test client that sends
a message to the echo server and then prints out the reply. The echo
protocol is defined in RFC 862. In short, we send a message to the
server, and the server echos the exact same message back.

Usage
-----

-  Add the the to the shell directory as xsh\_echotest.c
-  Modify shell.c and shell.h to include the echotest command.
-  Modify the Makefile in the shell directory to include xsh\_echotest.c
   and then make XINU from the compile directory
-  Boot XINU
-  Run netup command from the shell
-  Run echoclient from the shell with the arguments of the echoserver's
   ip and the message to be sent in quotes

   -  eg: echoclient 192.168.6.102 "Hello XINU World!"

xsh\_echotest.c
---------------

| ``#include <stddef.h>``
| ``#include <stdio.h>``
| ``#include <device.h>``
| ``#include <ether.h>``
| ``#include <tcp.h>``
| ``#include <string.h>``
| ````
| ``#define MSG_MAX_LEN 64``
| ``#define ECHO_PORT 7``
| ````
| ``/**``
| `` * Shell command (echotest) sends a message to an echo server per RFC 862.``
| `` * It waits for a response, then prints out the reply from the echo server.``
| `` * Expects args: echotest, echo server ip, Message to echo in quotes``
| `` * @param nargs number of arguments in args array``
| `` * @param args  array of arguments``
| `` * @return non-zero value on error``
| `` */``
| ``shellcmd xsh_echotest(int nargs, char *args[])``
| ``{``
| ``    ushort dev = 0;``
| ``    char buf[MSG_MAX_LEN];``
| ````
| ``    char *dest = args[1];``
| ````
| ``    struct netaddr dst;``
| ``    struct netaddr *localhost;``
| ``    struct netif *interface;``
| ````
| ``    int len;``
| ````
| ``    /* Allocate a new TCP device */``
| ``    if ((ushort)SYSERR == (dev = tcpAlloc()))``
| ``    {``
| ``        fprintf(stderr, "Client: Failed to allocate a TCP device.");``
| ``        return SYSERR;``
| ``    }``
| ````
| ``    /* Look up local ip info */``
| ``    interface = netLookup((ethertab[0].dev)->num);``
| ``    if (NULL == interface)``
| ``    {``
| ``        fprintf(stderr, "Client: No network interface found\r\n");``
| ``        return SYSERR;``
| ``    }``
| ``    localhost = &(interface->ip);``
| ````
| ``    /* Change the destination to ipv4 */``
| ``    if (SYSERR == dot2ipv4(dest, &dst))``
| ``    {``
| ``        fprintf(stderr, "Client: Failed to convert ip address.");``
| ``        return SYSERR;``
| ``    }``
| ````
| ``    /* Open the TCP device with the destination and echo port*/``
| ``    if (SYSERR == open(dev, localhost, &dst, NULL, ECHO_PORT, TCP_ACTIVE))``
| ``    {``
| ``        fprintf(stderr, "Client: Could not open the TCP device\r\n");``
| ``        return SYSERR;``
| ``    }``
| ````
| ``    /* Send the message to the destination*/``
| ``    memcpy(buf, args[2], MSG_MAX_LEN);``
| ````
| ``    if(SYSERR == write(dev, buf, MSG_MAX_LEN))``
| ``    {``
| ``        fprintf(stderr, "Client: Error writing packet to the network");``
| ``        close(dev);``
| ``        return SYSERR;``
| ``    }``
| ````
| ``    /* Read a response from the server */``
| ``    if(SYSERR != (len = read(dev, buf, MSG_MAX_LEN)))``
| ``    {``
| ``        printf("Client: Got response - %s\r\n", buf);``
| ``    }``
| ````
| ``    /* Close the device when done */``
| ``    close(dev);``
| ````
| ``    return 0;``
| ``}``
