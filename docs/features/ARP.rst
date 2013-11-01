ARP
===

As part of its :doc:`networking subsystem <Networking>`, XINU supports
the **Address Resolution Protocol** (**ARP**), which allows protocol
addresses (e.g. IPv4 addresses) to be translated into hardware
addresses (e.g. MAC addresses).

ARP daemon
----------

The ARP daemon is run automatically on start up and waits for incoming
ARP packets.  Incoming packets are filtered through :source:`netRecv()
<network/net/netRecv.c>` and ARP requests/replies are sent on to the
:source:`ARP Daemon <network/arp/arpDaemon.c>`.

Use of ARP when sending packets
-------------------------------

Callers of :source:`netSend() <network/net/netSend.c>` do not need to
specify ``hwaddr``, the hardware address of the destination computer,
and can leave this parameter as ``NULL``.  In such cases
:source:`arpLookup() <network/arp/arpLookup.c>` is called to try to
map the destination protocol address to a hardware address.  This
first searches the ARP table, but if the relevant entry is not found,
an ARP request is sent.  In the latter case, the calling thread is put
to sleep until the ARP daemon wakes it up after receiving the
corresponding reply, or until a designated timeout has elapsed.

Shell commands
==============

To print the ARP table, run the **arp** command from the :doc:`XINU
shell <Shell>`:

.. code-block:: none

    xsh@supervoc$ arp

    Address                 HWaddress               Interface
    192.168.6.10            52:54:03:02:B1:06       ETH0
    192.168.6.101           00:16:B6:28:7D:4F       ETH0
    192.168.6.130           00:25:9C:3A:87:53       ETH0

Currently there is no way to add/remove entries or clear the ARP table manually.

Resources
---------

* :rfc:`826`
