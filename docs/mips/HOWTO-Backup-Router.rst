Backing up your router
======================

.. note::

   This page appears to have been written with :doc:`WRT54GL` routers
   in mind and may or may not be applicable to other platforms.

When experimenting with kernels, you may find yourself corrupting the
router and having to recover from a known good state. It is a good idea
to make a copy of the CFE when it is in its original factory
configuration for if and when. Here are a few different methods
depending on your resources:

dd-wrt
------

If you are running a relatively recent version of
`dd-wrt <http://www.dd-wrt.com/>`__, you can download the CFE from the
web interface.

Location: ``http://ROUTER_IP/backup/cfe.bin>``

CFE
---

To backup the CFE from within the CFE itself you need an active
:doc:`serial console <HOWTO-Connect-to-Modified-Router>` and a live
network connection to a :doc:`TFTP server
</teaching/HOWTO-Deploy-Xinu>`. On the TFTP server, create a file for
the backup image. It must be world writable.  If you do not pre-create
the file for the TFTP client, the backup may fail. We name and label
our routers to keep them straight.  Consider naming the backup image
after each router's hostname if you have many of them.

Example::

    user@xinuserver:tftpboot$ touch routername.cfe
    user@xinuserver:tftpboot$ chmod a+w routername.cfe

Access the :doc:`CFE <Common-Firmware-Environment>` using the
:ref:`usual method <getting_into_cfe>`.

If necessary, configure the network interface and check the connection
to your TFTP server (192.168.6.10 in this example)::

    CFE> ifconfig eth0 -auto
    Device eth0:  hwaddr 00-1E-E5-86-02-7A, ipaddr 192.168.6.122, mask 255.255.255.0
            gateway 192.168.6.50, nameserver 134.48.7.10, domain xinu.mu.edu
    *** command status = 0
    CFE> ping 192.168.6.10
    192.168.6.10 (192.168.6.10) is alive
    192.168.6.10 (192.168.6.10): 1 packets sent, 1 received
    *** command status = 0

Save the CFE region to the file you created on the TFTP server::

    CFE> save 192.168.6.10:routername.cfe BC000000 40000
    262144 bytes written to 192.168.6.10:routername.cfe
    *** command status = 0

Note: You can also save the entire flash image (CFE+Kernel+NVRAM) by
passing 400000 as the length instead of 40000. This isn't really
necessary, but you might find it referenced elsewhere. If you have to
:doc:`recover <HOWTO-Recover-Router>` via JTAG it will take a LONG
time to restore the entire flash image. It is much more efficient to
load just the CFE, then upload the kernel via TFTP and use a factory
reset to restore NVRAM.

JTAG
----

For this method to work, we assume you have a working :doc:`EJTAG`
interface to your router. For details, check out our
:doc:`HOWTO-Recover-Router` page.

Use the ``-probeonly`` option to figure out what options TJTAG needs for
your particular setup.

Backup the CFE with TJTAG::

    user@host:tjtag$ ./tjtag -backup:cfe /wiggler /noemw /noreset

TJTAG will create a CFE.BIN file with some sort of timestamp identifier
appended to the filename. Rename and store somewhere safe.
