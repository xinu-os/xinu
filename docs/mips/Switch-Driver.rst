Switch driver
=============

Ethernet Port Numbering
-----------------------

On the back of a Linksys WRT54GL router, the Ethernet ports appear in
this order:

+-------+-----+-----+-----+-----+
| WAN   | 1   | 2   | 3   | 4   |
+-------+-----+-----+-----+-----+

Internally, the Ethernet ports are indexed in this order:

+-----+-----+-----+-----+-----+
| 4   | 3   | 2   | 1   | 0   |
+-----+-----+-----+-----+-----+

CFE and OpenWRT VLAN Configuration
----------------------------------

In order to use the Linksys WRT54GL routers for complex routing
purposes, both CFE and the operating system burned into Flash must be
modified to separate the four LAN ports into their own network
interfaces.

By default, the four LAN ports (0-3) are part of the same VLAN in CFE.
(Port 5 is internal to the CPU.)

| ``CFE> nvram get vlan0ports``
| ``3 2 1 0 5*``
| ``*** command status = 0``

This needs to be changed so that only one LAN port is in the VLAN.
Select an appropriate port number based on the diagram above. The port
that remains in the VLAN should be the one connected to the Xinu server.

| ``CFE> nvram set vlan0ports="0 5*"``
| ``*** command status = 0``
| ``CFE> nvram commit``
| ``*** command status = 0``

The network configuration on OpenWRT must also be changed.

``root@OpenWrt:/# vi /etc/config/network``

The file will originally contain this section.

| ``#### VLAN configuration``
| ``config switch eth0``
| ``        option vlan0    "0 1 2 3 5*"``
| ``        option vlan1    "4 5"``

It should be changed to look like this where the port in vlan0 is the
one connected to the Xinu server.

| ``#### VLAN configuration``
| ``config switch eth0``
| ``        option vlan0    "0 5*"``
| ``        option vlan1    "4 5"``
