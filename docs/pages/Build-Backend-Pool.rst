Build a Backend Pool
====================

Summary
-------

This page details how to scale your laboratory environment up to a pool
of backend target machines available for remote access.

The Big Picture
---------------

Image:XINU-Lab-schematic.gif

XINU Backends
~~~~~~~~~~~~~

Backend targets upload a student's kernel over a private network on
boot, and run the O/S directly. No simulations or emulation are
involved; this is real hardware.

MIPS targets: We use Linksys WRT54GL wireless routers (~$60) with
`serial port modifications <HOWTO:Modify_the_Linksys_hardware>`__ (~$10)
running an embedded MIPS32 200MHz processor, 4 MB flash, 16 MB RAM, two
UARTs, wired and wireless network interfaces.

PowerPC targets: We use Apple G3 desktops (recycled) with 512 MB RAM,
linear framebuffer, PCI bus, NIC, HD. Apple G4 MiniMac also supported.

CISC targets: Classic XINU runs on Intel x86, Sun 3/Motorola 68K, Sparc,
and VAX, among others.

XINU Server
~~~~~~~~~~~

A general purpose server with multiple network interfaces manages a
private network for the XINU backends, using standard network protocols
like DHCP and TFTP.

Backend serial consoles can connect directly to server's serial ports,
or, in larger installations, to a serial annex or concentrator that
allows many more serial ports.

A daemon running on the server allows users on frontend workstations to
remotely access backend serial consoles, or upload fresh kernels.
Optional rebooting hardware allows clients to remotely reset crashed
backends.

Our `Console Tools <Console Tools>`__ are freely available for modern
UNIX platforms, including Fedora Linux and Solaris.

XINU Frontends
~~~~~~~~~~~~~~

General purpose computer laboratory workstations can compile the XINU
kernel, using a standard GNU C compiler and UNIX toolchain. GCC
`cross-compilers <HOWTO:Build_and_Deploy_XINU>`__ are readily available
when the frontend architecture does not match the backend architecture.

Backend consoles can be `connected
directly <HOWTO:Connect_to_a_modified_router>`__ to frontend serial
ports, or frontends can communicate with the server daemon that manages
collections of backend serial consoles.

With fully remote console access, kernel upload and powercycling, any
machine on the network is a potential frontend, and need not be
physically near the XINU server and laboratory hardware. Students can
work on their operating system projects from their dorm room computers.

Additional (Optional) hardware
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Terminal Annex (EtherLite 32)
-  Serial-Controlled Power Strip (BayTech)
-  `Serial adapter diagrams <Serial adapter diagrams>`__

Setting Up the Server
---------------------

Our XINU Server is a PowerPC G5 XServe running Fedora Core Linux. We use
this configuration as a model for the information below, but other
architecture / O/S combinations are known to work, and there's no reason
this shouldn't work for virtually any machine with two network
interfaces running a modern UNIX O/S.

The first step in setting up your XINU server is to choose a machine for
your server (preferably the one you've been using for the first sections
of this tutorial) and download our `XINU Console
Tools <Downloads#Xinu_Console_Tools>`__. **NOTE:** some of the following
instructions require *root* access on the XINU server machine. After
downloading the RPM package, but before installing it, you will need to
install a few packages that should be available through your system's
package installing utility. On our machine we use the YUM package
installer. You will need to install the *tcp\_wrappers*,
*tcp\_wrappers-devel*, and *expect* packages. We used the commands
`` yum install tcp_wrappers``, `` yum install tcp_wrappers-devel``, and
`` yum install expect``.

After these packages are installed you can install the XINU Console
Tools. First navigate to the directory with the RPM file and execute the
command `` rpmbuild --rebuild xinu-console-latest.src.rpm``. This will
create four RPM files: `` xinu-console-server-2.05-3.i386.rpm``,
`` xinu-console-clients-2.05-3.i386.rpm``,
`` xinu-console-powerd-2.05-3.i386.rpm``,
`` xinu-console-debuginfo-2.05-3.i386.rpm `` (**NOTE:** the version
numbers in these files could be different). On our machine these files
were created in the directory `` /usr/src/redhat/RPMS/i386/``.

To get things up and running you will just need to install the server
and client packages. Navigate to the directory where the four RPM files
were created and execute the following commands:
`` rpm -iv xinu-console-server-2.05-3.i386.rpm `` and
`` rpm -iv xinu-console-clients-2.05-3.i386.rpm``. You now have all the
necessary tools installed to run your XINU server. You just have to make
some changes to some configuration files.

DHCP Daemon
-----------

Many modern firmware implementations will allow a device to
automatically acquire an IP address using the DHCP protocol even before
the O/S kernel begins to boot. The CFE on our Linksys backends will
attempt to configure its primary ethernet interface when issued the
command,

``  ifconfig -auto eth0``

over the serial console. See HOWTO:Deploy_Xinu for more details.

In our configuration, the XINU Server runs a DHCP daemon that is
configured to supply addresses to backends on the private network. We
use the standard **dhcp** server package that comes stock with our Linux
distribution (dhcp-3.0.5-3.fc6, as of this writing). Here is a sample
configuration file,
`dhcpd.conf <http://www.mscs.mu.edu/~brylow/xinu/Morbius-dhcpd.conf>`__.
Our configuration supplies a fixed IP address for each backend, based
upon MAC address.

You will need to change `` dhcp.conf `` file to match with your backend
pool. This requires knowing the MAC addresses of all your backend
routers and coming up with distinct fixed IP addresses for each one as
well. Also, note that the line `` range 192.168.1.200 192.168.1.220; ``
designates a range of IP addresses to be handed out to machines
requesting an IP address that do not have MAC addresses on the list.

It is important to note that the "filename" field designates a unique
boot image for each backend; this allows each backend to boot a distinct
image, customized by the student currently connected to that backend's
serial console.

To get this daemon up and running use the command
`` service dhcpd start `` and remember to restart it after every change
to the `` dhcp.conf `` file with the command `` service dhcp restart``.

TFTP Daemon
-----------

Many modern firmware implementations will allow a device to upload a
boot image over a network device using the Trivial File Transfer
Protocol (TFTP). We use the stock TFTP server available with our Linux
distribution (tftp-server-0.42-3.1, at this writing,) configured to
answer requests on the private network, and with the /tftpboot directory
writable by the xinu-console daemon user ID. Most TFTP daemons use TCP
wrapper to regulate access; see the notes on security below.

For your server, you will need to configure the permissions of the
`` /tftpboot `` directory (or whatever directory your TFTP server allows
client access to) so that the Xinu Console Daemon has writeable access
to it. This allows the Xinu Console Daemon to save boot images for the
routers to boot off of using the TFTP protocol. To do this you can run
the command `` chgrp -R xinu /tftpboot `` followed by the command
`` chmod g+w /tftpboot `` where `` /tftpboot `` is your TFTP server
directory. **NOTE:** you may need root access to make these changes.

If your XINU server is running a *SE Linux* firewall (ours with a fresh
install of Fedora 9 came running this firewall by default) you may have
to configure it to allow clients to access your XINU server's **TFTP
Server** running through *xinetd*. The simplest solution is to just tell
the *SE Linux* firewall to allow any activity over the network
connection your XINU server is using to connect to the backends. Our
XINU server is set up with two network interfaces and configured so that
`` eth0 `` is our network connection to the outside world and `` eth1 ``
is our connection to our backend pool. To tell the firewall to accept
all activity over our network connection with the backend pool we added
the line `` iptables -I INPUT -i eth1 -j ACCEPT `` to the
`` /etc/rc.local `` file. This will automatically run that command every
time the system boots up. **NOTE:** this could be very dangerous because
the connection between your front end and back end is now **insecure**.
This should only be done if you trust all possible users of the backend
pool because they now have unchecked access to your XINU server.

See our `Configuring the TFTP
Server <HOWTO:Deploy_Xinu#Configuring_the_TFTP_Server>`__ wiki for more
information on changing the configurations of the TFTP server.

Xinu Console Daemon
-------------------

The Xinu Console Daemon and various associated utilities provide network
clients with connectivity to backend consoles that are really only
connected directly to the console host. It is freely available from the
`downloads <downloads#Xinu_Console_Tools>`__ page.

Allow Frontend Machines to Access the Server
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

First, you will need to set up some configuration information to allow
frontend clients to interact with the Xinu Console Daemon running on the
XINU server. Open up the file `` /etc/rc.local `` and add the following
lines to the bottom of the file:

::

    TRUSTED_NET="134.48.6.0/255.255.255.0"
    XINUD_PORT="2024"
    iptables -I INPUT -s $TRUSTED_NET -p tcp --destination-port 1024:65535 -j ACCEPT
    iptables -I INPUT -s $TRUSTED_NET -p udp --destination-port $XINUD_PORT -j ACCEPT

The *TRUSTED\_NET* variable specifies the network of frontend machines
that are allowed to access the Xinu Console Daemon on the XINU server.
In other words these machines can actually use the Xinu Console Daemon
to get a backend and interact with it. You will want to change this
variable to match with the range of IP addresses of the frontend
machines you want to give access to.

The *XINUD\_PORT* variable specifies the port on the XINU server that
the Xinu Console Daemon uses. This value will always be "2024" on any
machine running any Linux distribution.

The next line sets up the XINU server to allow incoming *tcp* packets
from any machine on the trusted network on any port above 1024 because
these are the ports that the Xinu Console Daemon expects to use to
communicate with frontend machines using our Xinu client programs.

The last line sets up the XINU server to allow incoming *udp* packets
from any machine on the trusted network communicating on the
*XINUD\_PORT* port.

The Xinu Console Daemon uses TCP wrappers to prevent unauthorized
access; see the notes on security below.

Give Xinu Console Daemon Permission to use the Serial Devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Next, you will need to allow the Xinu Console Daemon access to the
serial devices which connect the XINU server with the backends. To do
this you can change the group to which the devices belong to the
"*xinu*\ " group by running the command `` chgrp xinu `` followed a list
of the serial devices. An example of this command would be
`` chgrp xinu /dev/ttyS0 /dev/ttyS1 /dev/ttyUSB0 `` where this example
server has three backend routers connected with serial device 0, serial
device 1, and USB device 0. **NOTE:** you may need *root* access to make
these changes.

Configure Xinu Console Daemon
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get your XINU server up and running you will need to make some
changes to the configuration of the Xinu Console Daemon. **NOTE:** you
may need *root* access to make some of these changes. First open the
file `` /etc/profile.d/xinu.sh``. It should look like this:

::

    XINU_SERVERS="morbius"
    export  XINU_SERVERS

You will want to change "morbius" to match your XINU server's name.

Next, open the file `` /etc/profile.d/xinu.csh``. It should look like
this:

::

    setenv XINU_SERVERS morbius

Again, you will want to change "morbius" to match your XINU server's
name. The changes to these two files ensures that the default value for
the *XINU\_SERVER* environment variable will be correct when remote
users log into the server. **NOTE:** you may need to use the fully
qualified domain name of the computer (for example
"morbius.mscs.mu.edu") for the Xinu Console Daemon to function
correctly.

Then, open the Xinu Console Daemon configuration file
`` /etc/xinu-consoled.conf``. Here is a sample of the configuration
file:

::

    #
    # This is the configuration file for the connection server.
    #
    # Lines beginning with # are comments.  Each line specifies a connection
    # and has the following form:
    #    name class path [ arguments ]*
    #
    # where
    #       name:           name of connection
    #       class:          the class of the connection
    #       path:           program to run when connection made
    #       arguments:      arguments to the program
    #
    # Each connection should be listed on a separate line
    #
    #-------------------------- Connections ------------------------------

    hostname:
    #---------------------------------------------------------------------

    router1 mips /usr/sbin/tty-connect -r 115200 /dev/ttyS0
    router1-dl DOWNLOAD   /usr/sbin/cp-download /tftpboot/router1.boot
    router1-pc POWERCYCLE /usr/bin/xinu-power r01
    router1-pf POWEROFF   /usr/bin/xinu-power d01
    router1-pn POWERON    /usr/bin/xinu-power u01


    router2 mips /usr/sbin/tty-connect -r 115200 /dev/ttyS1
    router2-dl DOWNLOAD   /usr/sbin/cp-download /tftpboot/router2.boot
    router2-pc POWERCYCLE /bin/echo "Turn off the router, then turn it on"
    router2-pf POWEROFF   /bin/echo "Turn off the router"
    router2-pn POWERON    /bin/echo "Turn on the router"

The line `` hostname: `` will need to contain your XINU server's name.
Following that line will be groups of configuration information for each
of the backends connected to your XINU server.

The first line of each entry contains the name of the backend, the
architecture it runs on, and the command (along with its arguments) for
the server to run in order to connect to the backend. So the line
`` router1 mips /usr/sbin/tty-connect -r 115200 /dev/ttyS0 `` tells the
Xinu Console Daemon that there is a router called "*router1*\ " that
runs a MIPS processor and that to connect to the backend router the
daemon should use `` tty-connect `` with a baudrate of 115200 on serial
device `` /dev/ttyS0``. So, when creating your own configuration file
you will want to edit the first line of each entry to reflect the name
of each of your backend routers and the serial device connecting it to
your XINU server. The other parts of the line should already be correct
and should not have to be changed for any of your backend routers.

The second line of each entry contains the information regarding what
actions the server needs to perform to download the specific boot image
for that backend router. So the line
`` router1-dl DOWNLOAD   /usr/sbin/cp-download /tftpboot/router1.boot ``
tells the Xinu Console Daemon that the *DOWNLOAD* command for
"*router1*\ " will run the program `` /usr/sbin/cp-download `` which
will take a user specified file on the frontend machine (usually their
`` xinu.boot `` file) and copy it into the XINU server's `` /tftpboot ``
directory as the file `` router1.boot``. This lets a student create
their own modified `` xinu.boot `` image and then, when the server
designates a backend for them to use, the Xinu Console Daemon will copy
their boot image onto the server with the specific name of the boot
image that will always run on that given backend. For your server you
will want to edit the second line of each entry so that the name of the
connection and the name of the boot image matches the name of that
backend router by changing `` router1-dl `` to `` [backend name]-dl ``
and by changing `` /tftpboot/router1.boot `` to
`` /tftpboot/[backend name].boot``.

The third, fourth, and fifth lines of each entry contain information
regarding what actions the server needs to perform to powercycle, power
off, and power on each router. However, without special hardware to
control the power of the backend pool (such as a Serial-Controlled Power
Strip) and special software like our Xinu Power Daemon to oversee the
use of that hardware these lines will be useless. If you have a more
advanced setup with a way to power on and off the backends remotely,
then these lines are where you specify the commands used by the Xinu
Console Daemon and Xinu Power Daemon to perform those actions. For more
information on setting this up, check out our `Xinu Power
Daemon <Xinu Power Daemon>`__ wiki. If you are not using any special
power control hardware then the user will be responsible for turning on
and off the routers by hand.

The Client
----------

This XINU server setup allows for frontend client machines to connect to
and run their own Xinu boot images on backends that are really only
connected to the XINU server. First, you will need to make sure that
each frontend machine has downloaded and installed the `Xinu Console
Tools <Downloads#Xinu_Console_Tools>`__ client package and that the IP
address of the frontend machine is in the *trusted network* set up in
the XINU server's `` /etc/rc.local `` file. Also, you will have to make
the same changes to the `` /etc/profile.d/xinu.sh `` and
`` /etc/profile.d/xinu.csh `` files that you made in the previous steps.
That means changing the *XINU\_SERVERS* variable in each file to match
with your XINU server's name.

Console Access
~~~~~~~~~~~~~~

Clients use the `` xinu-console `` program to connect their frontend
machines to backend routers through the XINU server running the Xinu
Console Daemon. To run this program just execute the following command:
`` xinu-console``. You may also pass as an argument to this program the
name of the specific backend you wish to connect to. Once the Xinu
Console Daemon has handed the frontend machine a backend to work on and
used **tty-connect** to establish a serial connection, the user can use
the `` xinu-console `` program to interact with the backend by using
some built-in commands and also by sending characters over the serial
connection by just typing on the keyboard. Here is the *help* output for
the built-in commands:

::

    h, ?    : help message
    b       : send break
    c       : continue session
    z       : suspend
    d       : download image
    p       : powercycle backend
    n       : power on backend
    f       : power off backend
    s       : spawn a program
    x       : quit and leave power on
    q       : quit and power down

Notice the key words "*download*\ ", "*powercycle*\ ", "*power on*\ ",
and "*power off*\ " and how they match up with the commands listed in
the Xinu Console Daemon configuration file `` xinu-consoled.conf``.
Built-in xinu-console commands with these key words in them will call
the associated program listed in the Xinu Console Daemon configuration
file for that specific backend. To use these commands first the user
will have to enter *command mode* by pressing Ctrl+Space. The user
should see the words `` (command-mode)``, letting them know that the
next character they enter will be interpreted by the xinu-console
program as a built-in command and not as just another character to send
across the serial connection to the backend. To quit out of the
`` xinu-console `` program, enter command mode and then type either "x"
or "q".

Mips-Console Wrapper Script
~~~~~~~~~~~~~~~~~~~~~~~~~~~

So, though it is entirely possible to just use the `` xinu-console ``
program to connect to a backend and manually send it all the commands to
boot XINU, it includes a lot of repetitive actions. So we have included
a script for automating the process of booting XINU on a MIPS backend
(like our LinkSys routers). This is our `` mips-console `` wrapper
script located in the file `` /usr/bin/mips-console``. Here is a copy of
the `mips-console file <mips-console>`__. In order to get this script to
run with your XINU server you will need to modify the top line
`` set ip 192.168.1.2 `` to match the IP address of the network
interface your XINU server uses to connect with your backend pool.

Once this change is complete, frontend users can navigate to the
directory that contains their `` xinu.boot `` image and simply run the
command `` mips-console `` to receive, connect to, and boot their own
modified XINU image on a backend router from the pool using the
completely automated script. It will automatically hand out a backend
router to the user from the pool and then download their `` xinu.boot ``
image to the XINU server's `` /tftpboot `` directory under the
appropriate name for the specific backend router. Then it will run that
backend's *powercycle* command and send breaks to get to the *CFE*
prompt. **NOTE:** if you do not have a hardware rebooter that can be
used to remotely turn off and on the backend, then the frontend user
will have to manually restart the router at this step. Next it will
automatically configure the backend's IP address using the XINU server's
**DHCP Server Daemon** and then boot off of the backend's specific boot
image located in the `` /tftpboot `` directory on the XINU server which
should now be the same image that the frontend user developed.

Category:HOWTO
