/**
 * @file xsh_kexec.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <conf.h>
#include <device.h>
#include <kexec.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>

#if NETHER
#  include <tftp.h>
#  include <dhcpc.h>
#  include <network.h>
#endif

static void usage(const char *command);

static void kexec_from_network(int netdev);
static void kexec_from_uart(int uartdev);

/**
 * @ingroup shell
 *
 * Kernel execute- Transfer control to a new kernel.
 */
shellcmd xsh_kexec(int nargs, char *args[])
{
    int dev;

    /* Output help, if '--help' argument was supplied */
    if (2 == nargs && 0 == strcmp(args[1], "--help"))
    {
        usage(args[0]);
        return SHELL_OK;
    }

    if (3 != nargs)
    {
        fprintf(stderr, "ERROR: Wrong number of arguments.\n");
        usage(args[0]);
        return SHELL_ERROR;
    }

    if (0 == strcmp(args[1], "-n"))
    {
        dev = getdev(args[2]);
        if (SYSERR == dev)
        {
            fprintf(stderr, "ERROR: device \"%s\" not found.\n", args[2]);
            return SHELL_ERROR;
        }
    #if NETHER
        if (dev < ETH0 || dev >= ETH0 + NETHER)
    #endif
        {
            fprintf(stderr, "ERROR: \"%s\" is not a valid network device.\n",
                    args[2]);
            return SHELL_ERROR;
        }
        kexec_from_network(dev);
    }
    else if (0 == strcmp(args[1], "-u"))
    {
        dev = getdev(args[2]);
        if (SYSERR == dev)
        {
            fprintf(stderr, "ERROR: device \"%s\" not found.\n", args[2]);
            return SHELL_ERROR;
        }
    #if NUART
        if (dev < SERIAL0 || dev >= SERIAL0 + NUART)
    #endif
        {
            fprintf(stderr, "ERROR: \"%s\" is not a valid UART device.\n",
                    args[2]);
            return SHELL_ERROR;
        }
        kexec_from_uart(dev);
    }
    else
    {
        usage(args[0]);
    }

    return SHELL_ERROR;
}

static void usage(const char *command)
{
        printf(
"Usage: %s\n\n"
"Description:\n"
"\tLoads and executes a new kernel.\n"
"Options:\n"
"\t-n <NETDEV>    Load the new kernel over the specified network device.\n"
"\t               This will bring down the corresponding network\n"
"\t               interface (if it's up), then use DHCP to get a network\n"
"\t               address and information about the TFTP server hosting\n"
"\t               the boot file.  The boot file (new kernel) will then be\n"
"\t               downloaded using TFTP and executed.\n"
#ifdef _XINU_PLATFORM_ARM_RPI_
"\t-u <UARTDEV>   Load the new kernel over the specified UART device.\n"
"\t               This is currently a Raspberry-Pi specific feature\n"
"\t               and is designed to be used with \"raspbootcom\"\n"
"\t               running on the other end of the serial connection.\n"
#endif
"\t--help         display this help and exit\n"

        , command);
}

static void kexec_from_network(int netdev)
{
#if defined(WITH_DHCPC) && NETHER != 0
    struct dhcpData data;
    int result;
    const struct netaddr *gatewayptr;
    struct netif *nif;
    void *kernel;
    uint size;
    char str_ip[20];
    char str_mask[20];
    char str_gateway[20];
    const char *netdevname = devtab[netdev].name;

    /* Bring network interface (if any) down.  */
    netDown(netdev);

    /* Run DHCP client on the device for at most 10 seconds.  */
    printf("Running DHCP on %s\n", netdevname);
    result = dhcpClient(netdev, 10, &data);
    if (OK != result)
    {
        fprintf(stderr, "ERROR: DHCP failed.\n");
        return;
    }

    /* Ensure the DHCP server provided the boot filename and TFTP server IP
     * address.  */
    if ('\0' == data.bootfile[0] || 0 == data.next_server.type)
    {
        fprintf(stderr, "ERROR: DHCP server did not provide boot file "
                "and TFTP server address.\n");
        return;
    }

    /* Bring up the network interface.  */
    netaddrsprintf(str_ip, &data.ip);
    netaddrsprintf(str_mask, &data.mask);
    if (0 != data.gateway.len)
    {
        netaddrsprintf(str_gateway, &data.gateway);
        printf("Bringing up %s as %s with mask %s (gateway %s)\n",
               netdevname, str_ip, str_mask, str_gateway);
        gatewayptr = &data.gateway;
    }
    else
    {
        printf("Bringing up %s as %s with mask %s (no gateway)\n",
               netdevname, str_ip, str_mask);
        gatewayptr = NULL;
    }
    result = netUp(netdev, &data.ip, &data.mask, gatewayptr);
    if (OK != result)
    {
        fprintf(stderr, "ERROR: failed to bring up %s.\n", netdevname);
        return;
    }
    nif = netLookup(netdev);

    /* Download new kernel using TFTP.  */
    netaddrsprintf(str_ip, &data.next_server);
    printf("Downloading bootfile \"%s\" from TFTP server %s\n",
           data.bootfile, str_ip);
    kernel = (void*)tftpGetIntoBuffer(data.bootfile, &nif->ip,
                                      &data.next_server, &size);

    if (SYSERR == (int)kernel)
    {
        fprintf(stderr, "ERROR: TFTP failed.\n");
        return;
    }

    /* Execute the new kernel.  */
    printf("Executing new kernel (size=%u)\n", size);
    sleep(100);  /* Wait just a fraction of a second for printf()s to finish
                    (no guarantees though).  */
    kexec(kernel, size);

    fprintf(stderr, "ERROR: kexec() returned!\n");

#else /* WITH_DHCPC && NETHER != 0 */
    fprintf(stderr,
            "ERROR: Network boot is not supported in this configuration.\n"
            "       Please make sure you have enabled one or more network\n"
            "       devices, along with the DHCP and TFTP clients.\n");
#endif /* !(WITH_DHCPC && NETHER != 0) */
}

static void kexec_from_uart(int uartdev)
{
#ifdef _XINU_PLATFORM_ARM_RPI_
    irqmask im;
    device *uart;
    ulong size;
    void *kernel;
    uchar *p;
    ulong n;

    im = disable();

    uart = (device*)&devtab[uartdev];

    /* Tell raspbootcom to send the new kernel.  */
    kputc('\x03', uart);
    kputc('\x03', uart);
    kputc('\x03', uart);

    /* Receive size of the new kernel.  */
    for (;;)
    {
        size = (ulong)kgetc(uart);
        size |= (ulong)kgetc(uart) << 8;
        size |= (ulong)kgetc(uart) << 16;
        size |= (ulong)kgetc(uart) << 24;
        if (size <= 99999999 && size != 0)
        {
            break;
        }

        /* Tell raspbootcom to re-send the size.  */
        kputc('S', uart);
        kputc('E', uart);
    }

    /* Tell raspbootcom the size was successfully received.  */
    kputc('O', uart);
    kputc('K', uart);

    /* Allocate buffer for new kernel.  */
    kernel = memget(size);
    if (kernel == (void*)SYSERR)
    {
        return;
    }

    /* Load new kernel over the UART, placing it into the buffer.  */
    p = kernel;
    n = size;
    while (n--)
    {
        *p++ = kgetc(uart);
    }

    /* Execute the new kernel.  */
    kexec(kernel, size);

    /* The following code should never actually be reached.  */
    memfree(kernel, size);
    restore(im);
#else /* _XINU_PLATFORM_ARM_RPI_ */
    fprintf(stderr, "ERROR: kexec from UART not supported on this platform.\n");
#endif /* !_XINU_PLATFORM_ARM_RPI_ */
}
