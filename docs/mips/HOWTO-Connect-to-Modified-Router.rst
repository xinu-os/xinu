Connecting to a modified router
===============================

Summary
-------

This will explain how to connect to the serial ports on a modified
Linksys WRT54G using serial communication software. In our tutorial we
will use Picocom for our serial communication software because it is
free and easy to get with a front end or server running Linux.

Before Starting
---------------

Expose a serial port on the router
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You must have successfully modified a Linksys WRT54G to expose at least
its first serial port in such a way that you can connect it to another
machine with serial communications software. If you have not done so
yet, please see :doc:`HOWTO-Modify-the-Linksys-Hardware`.

Acquire serial communication software
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There is a freely available software package for serial communication on
almost every major platform. Picocom is one such piece of software that
is easy to obtain on a machine running Linux. To get Picocom installed
on your front end simply open a terminal and type
`` yum install picocom  `` (or use your system's package install command
if it does not support the `` yum `` command). **NOTE:** you may need
*root* access to your front end machine in order to install packages. We
installed Picocom on a front end machine running Fedora 9 and got the
following output::

    [root@argolis ~]# yum install picocom
    Loaded plugins: refresh-packagekit
    Setting up Install Process
    Parsing package install arguments
    Resolving Dependencies
    --> Running transaction check
    ---> Package picocom.i386 0:1.4-4.fc9 set to be updated
    --> Finished Dependency Resolution

    Dependencies Resolved

    =============================================================================
     Package                 Arch       Version          Repository        Size
    =============================================================================
    Installing:
     picocom                 i386       1.4-4.fc9        fedora             29 k

    Transaction Summary
    =============================================================================
    Install      1 Package(s)
    Update       0 Package(s)
    Remove       0 Package(s)

    Total download size: 29 k
    Is this ok [y/N]: y
    Downloading Packages:
    (1/1): picocom-1.4-4.fc9.i386.rpm                        |  29 kB     00:00
    Running rpm_check_debug
    Running Transaction Test
    Finished Transaction Test
    Transaction Test Succeeded
    Running Transaction
      Installing: picocom                      ######################### [1/1]

    Installed: picocom.i386 0:1.4-4.fc9
    Complete!
    [root@argolis ~]#

Alternatively, if you are building multiple backends to be made
available as a pool, our suite of XINU `Console Tools <Console Tools>`__
includes a basic serial console utility called **tty-connect**. However
it is recommended that you do **not** use the XINU `Console
Tools <Console Tools>`__'s **tty-connect** utility for directly
connecting a single back end router to a front end machine because this
utility does not allow the user to send a ``ctrl-C`` command over the
serial connection, which is necessary in the upcoming steps in order to
properly communicate with your router.

Steps to Connect to the Router
------------------------------

Task One: Connect Serial (& Optionally Network) Cable(s)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ensure that the connection is going from UART0 (the first serial
port--if you followed our instructions on :doc:`modifying the router
<HOWTO-Modify-the-Linksys-Hardware>` it will be the **DB9 Female**
serial port on the left) as this is where the console will be running.
If you are connecting a standard PC serial port (a DTE) to your
router, use a straight serial cable. Other arrangements may require a
:doc:`Null Modem <Serial-Adapter-Diagrams>`; check your
transmit/receive line polarities to be sure.

Also, because the goal is to upload custom code to the router, it would
be a good idea to connect the router to your network by wiring it up via
one of the numbered LAN ports on the back of the router (NOT the
Internet/WAN port). This is not necessary to simply connect and
communicate with the router, but it is necessary if you want to try and
boot the router running your own custom code (which is the point of this
Xinu lab after all).

Task Two: Configure your Serial Communication Software
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The connection used by the router's serial port is fairly standard:
115200bps, with 8 data bits, no parity bit, and 1 stop bit, or 8N1. Set
your software to connect using these settings.

If you are following our tutorial and using Picocom as your serial
connection software, the command to open Picocom with these settings is
`` picocom -b 115200 /dev/ttyS0 `` (where `` /dev/ttyS0 `` is the name
of your front end's serial communication device hooked up to the
router). By default the other necessary settings are already set on
picocom; it's default connection uses 8 data bits, no parity bits, and 1
stop bit. If you use picicom to set up a connection you should get
output like the following::

    [root@argolis ~]# picocom -b 115200 /dev/ttyS0
    picocom v1.4

    port is        : /dev/ttyS0
    flowcontrol    : none
    baudrate is    : 115200
    parity is      : none
    databits are   : 8
    escape is      : C-a
    noinit is      : no
    noreset is     : no
    nolock is      : no
    send_cmd is    : ascii_xfr -s -v -l10
    receive_cmd is : rz -vv

    Terminal ready

Task Three: Power up the Router
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Yes, that means plug it in.

With serial communications software listening, you should see something
like the following output::

    CFE version 1.0.37 for BCM947XX (32bit,SP,LE)
    Build Date: Mon Nov 14 18:06:25 CST 2005 (root@localhost.localdomain)
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

    mac_init(): Find mac [00:18:39:6F:78:15] in location 0
    Nothing...

    eou_key_init(): Find key pair in location 0
    The eou device id is same
    The eou public key is same
    The eou private key is same
    Device eth0:  hwaddr 00-18-39-6F-78-15, ipaddr 192.168.1.1, mask 255.255.255.0
            gateway not set, nameserver not set
    Loader:raw Filesys:raw Dev:flash0.os File: Options:(null)
    Loading: ...... 1601536 bytes read
    Entry at 0x80001000
    Closing network.
    Starting program at 0x80001000
    CPU revision is: 00029008
    Primary instruction cache 16kb, linesize 16 bytes (2 ways)
    Primary data cache 8kb, linesize 16 bytes (2 ways)
    Linux version 2.4.20 (root@localhost.localdomain) (gcc version 3.2.3 with Broadcom modifications)
    ...
    (snip)
    ...
    Hit enter to continue...

Pressing enter will give you a root shell::

    BusyBox v0.60.0 (2005.11.14-09:45+0000) Built-in shell (msh)
    Enter 'help' for a list of built-in commands.

    #

Task Four: Access the Common Firmware Environment CLI
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you reboot the router while holding CTRL+C on the serial console, you
will get a CFE prompt::

    CFE version 1.0.37 for BCM947XX (32bit,SP,LE)
    Build Date: Mon Nov 14 18:06:25 CST 2005 (root@localhost.localdomain)
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

    mac_init(): Find mac [00:18:39:6F:78:15] in location 0
    Nothing...

    eou_key_init(): Find key pair in location 0
    The eou device id is same
    The eou public key is same
    The eou private key is same
    Device eth0:  hwaddr 00-18-39-6F-78-15, ipaddr 192.168.1.1, mask 255.255.255.0
            gateway not set, nameserver not set
    Automatic startup canceled via Ctrl-C
    CFE> ^C
    CFE> ^C
    CFE>

See the CFE page for more information about using this prompt.

What to do next?
----------------

Now that you have successfully modified and connected to your router,
you are ready to :doc:`Build </Getting-Started>` and :doc:`Deploy
</teaching/HOWTO-Deploy-Xinu>` XINU.

Acknowledgements
----------------

*This work is supported in part by NSF grant DUE-CCLI-0737476.*
