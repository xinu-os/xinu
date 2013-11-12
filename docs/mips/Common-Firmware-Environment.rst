Common Firmware Environment
===========================

About
-----

The **Common Firmware Environment** (**CFE**) is the firmware
developed by Broadcom for the BCM947xx SoC platform (among others).
It is the first code that runs when the router boots and performs
functions similar to Apple's Open Firmware:

-  Initializes the system
-  Sets up a basic environment in which code can run
-  Optionally provides a command line interface non-standard usage
-  Loads and executes a kernel image (expecting to be jettisoned shortly
   thereafter)

So, in normal operation, a user will not see CFE working at all; it
will load the Linksys kernel and send it on its merry way without
hesitation.  For us, however, CFE is crucial, because it provides us
with the ability to load an image over the network using TFTP.

We have access to two major documents covering CFE, the reference
manual, and the functional specification. Much of the content in these
two documents overlaps.

.. _getting_into_cfe:

Getting into CFE
----------------

To get into CFE, it will be very helpful to enable "boot wait" from the
Administration Tab under the router's Web GUI. This will cause the
router to wait on startup for a signal to stop booting into the firmware
and enter CFE.

Once you have that set up and you've :doc:`connected to the router
<HOWTO-Connect-to-Modified-Router>`, just type "reboot" (assuming
OpenWRT is installed, it may be different for other firmwares) to
reboot the router. This can also be done by power-cycling the router.
As it's booting up, send a continuous stream of Ctrl+C characters to
cancel booting and you'll be entered right into CFE.

From there, you can prod around CFE's features or load your own kernel
using the command line interface.

Command Line Interface
----------------------

The CFE Command Line Interface (CLI) is a very simple "shell-like"
command prompt. It has a few environmental variables and minimal
functionality. However, it is complex enough power to load a boot
image over the network and begin executing code. The User interface is
described on page 19 of 145 in the `CFE documentation
<http://melbourne.wireless.org.au/files/wrt54/cfe.pdf>`__.

To get the to the CLI, you can use either the power-on method or load
OpenWRT and type reboot. The CFE boot screen looks like:

.. code-block:: none

    CFE version 1.0.37 for BCM947XX (32bit,SP,LE)
    Build Date: Fri Sep 23 17:46:42 CST 2005 (root@localhost.localdomain)
    Copyright (C) 2000,2001,2002,2003 Broadcom Corporation.

    Initializing Arena
    Initializing Devices.

    No DPN
    et0: Broadcom BCM47xx 10/100 Mbps Ethernet Controller 3.90.37.0
    CPU type 0x29008: 200MHz
    Total memory: 16384 KBytes

    Total memory used by CFE:  0x80300000 - 0x803A39C0 (670144)
    Initialized Data:          0x803398D0 - 0x8033BFE0 (10000)
    BSS Area:                  0x8033BFE0 - 0x8033D9C0 (6624)
    Local Heap:                0x8033D9C0 - 0x803A19C0 (409600)
    Stack Area:                0x803A19C0 - 0x803A39C0 (8192)
    Text (code) segment:       0x80300000 - 0x803398D0 (235728)
    Boot area (physical):      0x003A4000 - 0x003E4000
    Relocation Factor:         I:00000000 - D:00000000

    Boot version: v3.7
    The boot is CFE

    mac_init(): Find mac [00:16:B6:28:7D:4F] in location 0
    Nothing...

    eou_key_init(): Find key pair in location 0
    The eou device id is same
    The eou public key is same
    The eou private key is same
    Device eth0:  hwaddr 00-16-B6-28-7D-4F, ipaddr 192.168.1.1, mask 255.255.255.0
            gateway not set, nameserver not set
    Reading :: Failed.: Interrupted
    CFE> ^C
    CFE>

Of course, items like the hwaddr will be different from router to
router.

Once you have a command prompt, you can type ``help`` and get a listing
of commands available:

.. code-block:: none

    CFE> help
    Available commands:

    rndis               Broadcom USB RNDIS utility.
    et                  Broadcom Ethernet utility.
    modify              Modify flash data.
    nvram               NVRAM utility.
    reboot              Reboot.
    flash               Update a flash memory device
    memtest             Test memory.
    f                   Fill contents of memory.
    e                   Modify contents of memory.
    d                   Dump memory.
    u                   Disassemble instructions.
    autoboot            Automatic system bootstrap.
    batch               Load a batch file into memory and execute it
    go                  Verify and boot OS image.
    boot                Load an executable file into memory and execute it
    load                Load an executable file into memory without executing it
    save                Save a region of memory to a remote file via TFTP
    ping                Ping a remote IP host.
    arp                 Display or modify the ARP Table
    ifconfig            Configure the Ethernet interface
    show devices        Display information about the installed devices.
    unsetenv            Delete an environment variable.
    printenv            Display the environment variables
    setenv              Set an environment variable.
    help                Obtain help for CFE commands

    For more information about a command, enter 'help command-name'
    *** command status = 0
    CFE>

A command status of 0 is always a good thing, other command statuses are
errors.

The next two commands are very important to booting a custon kernel
image: ``ifconfig`` and ``boot``.

``ifconfig`` is just the Linux counterpart, it will set up the specified
interface. For our router, we have the switch portion of the router
connected to a xinu server (which is simply a TFTP and DHCP server).
From there we type ``ifconfig -auto eth0`` which will ask the xinu
server for an IP address and set up the router.

.. code-block:: none

    CFE> ifconfig -auto eth0
    Device eth0:  hwaddr 00-16-B6-28-7D-4F, ipaddr 192.168.5.2, mask 255.255.254.0
            gateway 192.168.5.220, nameserver 192.168.5.220, domain xinu.mu.edu
    *** command status = 0
    CFE>

We now have an IP address and can transfer our boot image.

For our purposes, we name our boot images after the unit on which it
will load (supervoc is our demo router).

.. code-block:: none

    CFE> boot -elf 192.168.5.220:supervoc.boot
    Loader:elf Filesys:tftp Dev:eth0 File:192.168.5.220:supervoc.boot Options:(null)
    Loading: 0x80001000/3145 0x80001c49/23 Entry at 0x80001000
    Closing network.
    Starting program at 0x80001000

Let's walk through these lines:

.. code-block:: none

    boot -elf 192.168.5.220:supervoc.boot

This will begin booting the ``supervoc.boot`` kernel that is located at
192.168.5.220 (our xinu server and, no, name resolution does not
work).

.. code-block:: none

    Loader:elf Filesys:tftp Dev:eth0 File:192.168.5.220:supervoc.boot Options:(null)

A fairly explanatory line stating the file type it is loading
(``elf``), the file system to be used (``tftp``), the device which it
is using to transfer the image (``eth0``), and where that image is
from (``192.168.5.220:supervoc.boot``).

.. code-block:: none

    Loading: 0x80001000/3145 0x80001c49/23 Entry at 0x80001000

This is also a line of explanation.  The first portion
(``0x80001000/3145``) tells us the 'physical' address of where we
begin loading our image and the size of the image (in bytes). Next is
the address of the end of the image (``0x80001c49/23``) and (I
believe) the amount of padding to make the image size base 16.  The
last part is the address which CFE will branch to upon completion of
upload, this is the start of your kernel.

.. code-block:: none

    Closing network.
    Starting program at 0x80001000

The closes the network and begins execution the code at address
``0x8000100``. Any lines of text outputted after this are from your
boot image (unless CFE throws an exception and shows a memory dump).
