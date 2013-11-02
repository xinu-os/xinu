Deploying Xinu
==============

By this point it is assumed you have :ref:`obtained a supported router
<supported_platforms>` to use as a back end, have :doc:`made the
necessary modifications </mips/HOWTO-Modify-the-Linksys-Hardware>` to
use the serial port, have :doc:`connected to your backend router
</mips/HOWTO-Connect-to-Modified-Router>` using serial communication
software (such as PICOCOM), and have :ref:`built a cross-compiler
<cross_compiler>` (from your host computer's architecture to MIPS) and
used it to :ref:`build a XINU boot image <compiling>` to run on the
back end (a file called ``xinu.boot`` should be in your ``compile/``
directory).

TFTP Server
-----------

Installing the TFTP Server Package
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first step in running your XINU image is to run a **TFTP Server**
on your front end so that it can transfer the boot image onto the back
end via the network connection. Getting a **TFTP Server** running on
your front end is a fairly simple process on Linux. Use your front
end's package install commands to search for a tftp-server package and
then install that package. Using Fedora 9 and the YUM package install
command, we entered the command ``yum list | grep tftp`` and obtained
the following output::

    [root@argolis compile]# yum list | grep tftp
    tftp.i386                                0.48-6.fc9             mulugupdates
    tftp-server.i386                         0.48-6.fc9             mulugupdates
    [root@argolis compile]#

This command tells the package installer to find any packages with
"tftp" in the title. We want the package called *tftp-server.i386*
because this will allow us to run a **TFTP-Server** on our front end
machine. Next, we need to actually install the package. Again we use
our package install command to install the package. Using Fedora 9 and
the YUM package install command, we entered the command ``yum install
tftp-server`` and obtained the following output::

    [root@argolis compile]# yum install tftp-server
    Loaded plugins: refresh-packagekit
    Setting up Install Process
    Parsing package install arguments
    Resolving Dependencies
    --> Running transaction check
    ---> Package tftp-server.i386 0:0.48-6.fc9 set to be updated
    --> Finished Dependency Resolution
    Dependencies Resolved
    =============================================================================
     Package                 Arch       Version          Repository        Size 
    =============================================================================
    Installing:
     tftp-server             i386       0.48-6.fc9       mulugupdates       35 k 
    Transaction Summary
    =============================================================================
    Install      1 Package(s)         
    Update       0 Package(s)         
    Remove       0 Package(s)         
    Total download size: 35 k
    Is this ok [y/N]: yes
    Downloading Packages:
    (1/1): tftp-server-0.48-6.fc9.i386.rpm                   |  35 kB     00:00    
    Running rpm_check_debug
    Running Transaction Test
    Finished Transaction Test
    Transaction Test Succeeded
    Running Transaction
      Installing: tftp-server                  ######################### [1/1] 
    Installed: tftp-server.i386 0:0.48-6.fc9
    Complete!
    [root@argolis compile]#

.. _tftpd_configuration:

Configuring the TFTP Server
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now that your **TFTP Server** package is installed, you need to
configure some settings to get it to run properly. *xinetd* is a server
daemon that can run many different types of servers. Here, though, we're
only interested in running a **TFTP Server**, so we want to configure
the daemon to run that. The following commands will configure the
*xinetd* daemon to run a **TFTP Server** on the front end: The command
``chkconfig xinetd on`` will configure the *xinetd* daemon to run on
the front end. The command ``chkconfig tftp on`` configures the daemon
to run a **TFTP Server** when it is running. The next command,
``service xinetd restart`` actually restarts the *xinetd* daemon to
make sure its up and running. After entering the previous commands we
got the following output::

    [root@argolis trunk]# chkconfig xinetd on
    [root@argolis trunk]# chkconfig tftp on
    [root@argolis trunk]# service xinetd restart
    Stopping xinetd:                                           [  OK  ]
    Starting xinetd:                                           [  OK  ]
    [root@argolis trunk]#

To verify that things worked correctly it is a good idea to type in
the command ``chkconfig --list`` and check to make sure *xinetd* is
running and that it is running a **TFTP Server**. You should get
output that looks like this::

     [root@argolis compile]# chkconfig --list
     NetworkManager  0:off   1:off   2:off   3:off   4:off   5:off   6:off
     acpid           0:off   1:off   2:on    3:on    4:on    5:on    6:off
     anacron         0:off   1:off   2:on    3:on    4:on    5:on    6:off
     atd             0:off   1:off   2:off   3:on    4:on    5:on    6:off
     auditd          0:off   1:off   2:on    3:on    4:on    5:on    6:off
     avahi-daemon    0:off   1:off   2:off   3:on    4:on    5:on    6:off
     ...
     ...
     ...
     winbind         0:off   1:off   2:off   3:off   4:off   5:off   6:off
     wpa_supplicant  0:off   1:off   2:off   3:off   4:off   5:off   6:off
     xinetd          0:off   1:off   2:off   3:on    4:on    5:on    6:off
     xinu-consoled   0:off   1:off   2:off   3:on    4:on    5:on    6:off
     ypbind          0:off   1:off   2:off   3:off   4:off   5:off   6:off
     yum-updatesd    0:off   1:off   2:on    3:on    4:on    5:on    6:off

     xinetd based services:
             chargen-dgram:  off
             chargen-stream: off
             cvs:            off
             daytime-dgram:  off
             daytime-stream: off
             discard-dgram:  off
             discard-stream: off
             echo-dgram:     off
             echo-stream:    off
             rsync:          off
             tcpmux-server:  off
             tftp:           on
             time-dgram:     off
             time-stream:    off
     [root@argolis compile]#

The important lines in this output are the *xinetd* line in the list of
services and the *tftp* line at the bottom section of xinetd based
services. Make sure that the *xinetd* line says "on" for 3 and 5. Also
make sure that the *tftp* line says "on".

By default the **TFTP Server** running on *xinetd* uses the directory
``/var/lib/tftpboot`` as its directory that will contain files on your
server that you wish to make accessible to tftp clients. We will
simply refer to this directory as the *tftp server directory* for the
remainder of the tutorial. Using this directory is fine, but if you
wish to change your *tftp server directory* the process is simple.
Opening up the tftp configuration file for *xinetd* (ours was found in
the path ``/etc/xinetd.d/tftp``) should give you the output::

    # default: off
    # description: The tftp server serves files using the trivial file transfer \
    #       protocol.  The tftp protocol is often used to boot diskless \
    #       workstations, download configuration files to network-aware printers, \
    #       and to start the installation process for some operating systems.
    service tftp
    {
            disable = no
            socket_type             = dgram
            protocol                = udp
            wait                    = yes
            user                    = root
            server                  = /usr/sbin/in.tftpd
            server_args             = -s /var/lib/tftpboot
            per_source              = 11
            cps                     = 100 2
            flags                   = IPv4
    }

You need to change the value of the ``server_args`` variable. Change
the value of ``server_args`` to point to the directory you wish to be
your *tftp server directory*. In our lab we use the directory
``/tftpboot``. You will need to remember your *tftp server directory*
because this is where you will need to put your ``xinu.boot`` file
that you want to run on the back end. After you've made the necessary
changes to the file ``/etc/xinetd.d/tftp``, it should look like this::

     # default: off
     # description: The tftp server serves files using the trivial file transfer \
     #       protocol.  The tftp protocol is often used to boot diskless \
     #       workstations, download configuration files to network-aware printers, \
     #       and to start the installation process for some operating systems.
     service tftp
     {
             socket_type             = dgram
             protocol                = udp
             wait                    = yes
             user                    = root
             server                  = /usr/sbin/in.tftpd
             server_args             = -s /tftpboot
             disable                 = no
             per_source              = 11
             cps                     = 100 2
             flags                   = IPv4
     }

If you are going to use our approach you may need to create a
``/tftpboot`` directory. To do this execute the following command:
``mkdir /tftpboot``. Anytime you make changes to the
``/etc/xinetd.d/tftp`` file you will need to restart your *xinetd*
daemon again with the command ``service xinetd restart``.

There are a couple other configurations that you may need to set up in
order to get your **TFTP Server** running. If your front end is running
an *iptables* firewall (our front end with a fresh install of Fedora 9
came running this firewall by default) you may have to configure it to
allow clients to access your front end's **TFTP Server** running through
*xinetd*. The simplest solution is to just tell the *iptables* firewall
to allow any activity over the network connection your front end is
using to connect to the back end. Our front end machine is set up with
two network cards and configured so that ``eth0`` is our network
connection to the outside world and ``eth1`` is our connection to our
back end. To tell the firewall to accept all activity over our network
connection with the backend we used the command
``iptables -I INPUT -i eth1 -j ACCEPT``. If you do not want to run this
command every time you restart your machine you can add the line to the
``/etc/rc.local`` file. This will automatically run that command every
time the system boots up. **NOTE:** this could be very dangerous because
the connection between your front end and back end is now **insecure**.
This should only be done if you trust all possible users of the backend
because they now have unchecked access to your frontend machine.

Also, you may need to configure your frontend's IP address. If you are
using a DHCP server for your frontend machine you can simply execute
``ifconfig eth0 -auto`` on your backend's CFE prompt to obtain an IP
for your backend automatically. However, most likely this is not the
case and you will have to make sure that the network connection that
your front end machine is using to communicate with the backend is
configured with a static IP address in the 192.168.1.[2-255] range
because your backend router will have a default IP address of
192.168.1.1 when it reboots. If it is the case that the IP address of
your frontend machine's network connection with the backend is set
outside of this range, then check out our quick tutorial for `changing
the IP address <HOWTO:Change your Frontend's IP Address>`__ of your
network connection.

The last step before actually trying to boot your backend with a XINU
image, is to copy the actual ``xinu.boot`` file to the *tftp server
directory*. This is the directory that the ``/etc/xinetd.d/tftp``
file's ``server_args`` variable points to. In our example we used the
directory ``/tftpboot``. This is the directory where you need to copy
the ``xinu.boot`` file. This is easily done by using a command like
``cp xinu.boot /tftpboot/xinu.boot``, assuming that your current
working directory contains the ``xinu.boot`` file and that
``/tftpboot`` is your *tftp server directory*.

.. _booting_xinu_on_backend:

Booting XINU on your Backend
----------------------------

By now your **TFTP Server** should be up and running correctly, your
``xinu.boot`` file should be in the correct directory so your backend
can access it through the TFTP protocol, and your frontend machine
should either be running as a DHCP server or (more likely) has a
static IP address in the 192.168.1.[2-255] range on its network
connection with the backend.

To boot your backend router running XINU, first make sure you are
connected to the backend with some serial communication software and
are at the CFE prompt. If you are not looking at the backend router's
CFE prompt follow the instructions on how to :doc:`connect to your
backend router </mips/HOWTO-Connect-to-Modified-Router>`. In the CFE
prompt type the command ``boot -elf [host ip]:xinu.boot`` where "[host
ip]" is the IP address of the frontend machine's connection to the
backend router.  If all has gone correctly the router should now be
running a XINU image and you will be greeted with a basic shell (xsh$
). On our frontend machine running Fedora 9 with an IP address of
``192.168.1.2`` we optained the following output::

    CFE> boot -elf 192.168.1.2:xinu.boot
    Loader:elf Filesys:tftp Dev:eth0  File:192.168.1.2:xinu.boot  Options:(null)
    Loading: 0x80001000/114724 0x8001d024/18480 Entry at 0x80001000
    Closing network.
    Starting program at 0x80001000
    (Mips XINU) #0 (root@argolis.mscs.mu.edu) Mon Jun 23 17:47:42 CDT 2008
      16777216 bytes physical memory.
          4096 bytes reserved system area.
        133204 bytes XINU code.
         32764 bytes stack space.
      16607152 bytes heap space.
    ______________________________________
        _    _  _____  ______   _     _ 
       \ \  / /(_____)|  ___ \ | |   | |
        \ \/ /    _   | |   | || |   | |
         )  (    | |  | |   | || |   | |
        / /\ \  _| |_ | |   | || |___| |
       /_/  \_\(_____)|_|   |_| \______|
    ______________________________________
    Welcome to the wonderful world of XINU!
    xsh$ 

Within the shell you have some basic commands: exit, gpiostat, help,
kill, memstat, led, ps, reset, sleep, uartstat. Each can be described
using [command] --help.

::

    xsh$ help
    Shell Commands:
            clear
            ethstat
            exit
            gpiostat
            help
            kill
            led
            memstat
            memdump
            ps
            reset
            sleep
            test
            testsuite
            uartstat
            nvram
            ?
    xsh$ 

Congratulations, You now have a working backend running XINU! You can
now make changes to the XINU code, recompile it using the ``make``
command to get a new ``xinu.boot`` image, copy that file into the
*tftp server directory*, and use the same ``boot -elf [host
ip]:xinu.boot`` command to run your very own, modified version of the
XINU operating system.

What to do next?
----------------

Now that you have successfully booted and run |EX| on your backend
router, you might want to try to :doc:`build a pool of backends
</teaching/HOWTO-Build-Backend-Pool>` to allow multiple users to each
run their own version of XINU on a different backend.

Acknowledgements
----------------

*This work is supported in part by NSF grant DUE-CCLI-0737476.*
