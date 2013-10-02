ARP
===

About
-----

XINU features an automatic address resolution protocol. The ARP daemon
is run automatically on start up and waits for incoming ARP packets.
Incoming packets are filtered through netRecv and ARP requests/replies
are sent on to the ARP Daemon.

On the other side, when sending packets, netSend will check the ARP
table to see if it has a hardware address for the given IP. If there is
no matching entry in the ARP table, an ARP request is sent and

To print the ARP table run the arp command from the XINU shell:
xsh@supervoc$ arp

| ``Address                 HWaddress               Interface``
| ``192.168.6.10            52:54:03:02:B1:06       ETH0``
| ``192.168.6.101           00:16:B6:28:7D:4F       ETH0``
| ``192.168.6.130           00:25:9C:3A:87:53       ETH0``

Currently there is no way to add/remove entries or clear the ARP table
manually.

Resources
---------

RFC 826
