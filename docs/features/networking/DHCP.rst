DHCP
====

**DHCP** (**Dynamic Host Configuration Protocol**) support has been
added to XINU as part of its :doc:`networking subsystem <index>`.
Currently, only IPv4 client support--- that is, acquiring IPv4 address
information--- is supported. The code is located in
:source:`network/dhcpc`, and the API is declared in
:source:`include/dhcpc.h`.

In addition to basic IPv4 information (assigned IPv4 address, netmask,
and gateway) the DHCP client returns the "bootfile" and "next-server"
options if they are provided by the DHCP server.  If specified, this
information can be used to download the boot file using XINU's
:doc:`TFTP client <TFTP>`.

.. note::
    This page refers specifically to the DHCP client support built
    into XINU, which is completely separate from the DHCP support
    included in CFE, which is third-party firmware.

Resources
---------

- :wikipedia:`Dynamic Host Configuration Protocol - Wikipedia <Dynamic Host Configuration Protocol>`
- :rfc:`2131`
