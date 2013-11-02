UDP
===

XINU supports the **User Datagram Protocol** (**UDP**).  The support
can be found in the :source:`device/udp/` directory.  Note that this
is *not* in the :source:`network/` directory; this is because the UDP
module is designed to provide **UDP devices** that can be controlled
using the generic XINU device functions such as ``open()``,
``control()``, ``read()``, ``write()``, ``close()``.

.. contents::
   :local:

Debugging
---------

The UDP module contains :doc:`Trace statements </development/Trace>`
for debugging.  To enable, uncomment the following line in
:source:`include/udp.h`, and optionally change the device (such as
TTY1) to which messages will be logged::

    // #define TRACE_UDP  TTY1

Example (TFTP client)
---------------------

See :source:`network/tftp/tftpGet.c`.  This supports a real protocol
(TFTP GET), although there are some complications in the code.

Example (client + server)
-------------------------

About
~~~~~

This is a simple example of how to use the UDP networking features in
XINU (version 2.0 and later).

Usage
~~~~~

-  Add the both of the files to the shell directory as ``xsh_udpclient.c``
   and ``xsh_udpserver.c``.
-  Modify ``system/shell.c`` and ``include/shell.h`` to include the
   udpclient and udpserver commands.
-  Modify ``shell/Makerules`` to include ``xsh_udpclient.c`` and
   ``xsh_udpserver.c``, then make XINU from the compile directory
-  Boot two separate XINU consoles
-  Run **netup** from the shell on both consoles
-  Run **udpserver** from the shell on the first console
-  Run **udpclient** from the shell on the second console with the arguments
   of the udpserver's ip and the message to be sent in quotes, for
   example:

        ``udpclient 192.168.6.102 "Hello XINU World!"``

``xsh_udpclient.c``
~~~~~~~~~~~~~~~~~~~

.. code:: c

    #include <stddef.h>
    #include <stdio.h>
    #include <device.h>
    #include <ether.h>
    #include <udp.h>
    #include <string.h>

    #define MSG_MAX_LEN 64
    #define ECHO_PORT 9989


    /**
    * Shell command (udpclient) runs a client that sends an ASCII
    * message over the network to a server using UDP.
    * Expects arg0 to be echoclient, args1 to be the destination IP
      address, args2 to be the message in quotes
    * @param nargs number of arguments in args array
    * @param args array of arguments
    * @return non-zero value on error
    */
    shellcmd xsh_udpclient(int nargs, char *args[])
    {
        ushort dev = 0;
        char buf[MSG_MAX_LEN];

        char *dest = args[1];

        struct netaddr dst;
        struct netaddr *localhost;
        struct netif *interface;


        /* Allocate a new UDP device */
        if ((ushort)SYSERR == (dev = udpAlloc()))
        {
            fprintf(stderr, "Client: Failed to allocate a UDP device.");
            return SYSERR;
        }

        /* Look up local ip info */
        interface = netLookup((ethertab[0].dev)->num);
        if (NULL == interface)
        {
            fprintf(stderr, "Client: No network interface found\r\n");
            return SYSERR;
        }
        localhost = &(interface->ip);

        /* Change the destination to ipv4 */
        if (SYSERR == dot2ipv4(dest, &dst))
        {
            fprintf(stderr, "Client: Failed to convert ip address.");
            return SYSERR;
        }

        /* Open the UDP device with the destination and echo port*/
        if (SYSERR == open(dev, localhost, &dst, NULL, ECHO_PORT))
        {
            fprintf(stderr, "Client: Could not open the UDP device\r\n");
            return SYSERR;
        }

        /* Send the message to the destination*/
        memcpy(buf, args[2], MSG_MAX_LEN);

        if(SYSERR == write(dev, buf, MSG_MAX_LEN))
        {
            close(dev);
            return SYSERR;
        }

        /* Closee the device when done */
        close(dev);

        return 0;
    }

``xsh_udpserver.c``
~~~~~~~~~~~~~~~~~~~

.. code:: c

    #include <stddef.h>
    #include <stdio.h>
    #include <device.h>
    #include <udp.h>
    #include <stdlib.h>
    #include <ether.h>
    #include <string.h>

    #define ECHO_PORT 9989

    /**
    * Shell command (udpserver) runs a UDP server that waits for an
    * incoming message, and then prints it out. Does not expect any
    * arguments.
    * @param nargs number of arguments in args array
    * @param args array of arguments
    * @return non-zero value on error
    */
    shellcmd xsh_echoserver(int nargs, char *args[])
    {
        ushort dev = 0;
        int len = 0;

        char buffer[UDP_MAX_DATALEN];

        struct netaddr *localhost;

        struct netif *interface;
        struct udpPseudoHdr *pseudo;
        struct udpPkt *udp;


        /* Allocate a new UDP device */
        if ((ushort)SYSERR == (dev = udpAlloc()))
        {
            fprintf(stderr, "Server: Failed to allocate a UDP device.\r\n");
            return SYSERR;
        }

        /* Look up local ip info */
        interface = netLookup((ethertab[0].dev)->num);

        if (NULL == interface)
        {
            fprintf(stderr, "Server: No network interface found\r\n");
            return SYSERR;
        }


        /* Open the UDP device using localhost and the echo port to listen to*/
        localhost = &(interface->ip);

        if (SYSERR == open(dev, localhost, NULL, ECHO_PORT, NULL))
        {
            fprintf(stderr, "Server: Could not open the UDP device\r\n");
            return SYSERR;
        }

        /* Set the UDP device to passive mode */
        if (SYSERR == control(dev, UDP_CTRL_SETFLAG, UDP_FLAG_PASSIVE, NULL))
        {
            kprintf("Server: Could not set UDP device to passive mode\r\n");
            close(dev);
            return SYSERR;
        }


        /* Read lop, wait for a new request */
        printf("Server: Waiting for message\r\n");

        while (SYSERR != (len = read(dev, buffer, UDP_MAX_DATALEN)))
        {
            pseudo = (struct udpPseudoHdr *)buffer;
            udp = (struct udpPkt *)(pseudo + 1);
            printf("Server: Received Message - %s\r\n", udp->data);
        }

            close(dev);

        return 0;
    }

Resources
---------

* :wikipedia:`User Datagram Protocol - Wikipedia <User Datagram Protocol>`
* :rfc:`768`
