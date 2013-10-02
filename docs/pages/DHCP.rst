Dynamic Host Configuration Protocol
===================================

**DHCP** (**Dynamic Host Configuration Protocol**) support has been
added to Embedded Xinu. Currently, only IPv4 client support--- that is,
acquiring IPv4 address information--- is supported. The code is located
in . The API includes the header declaring ``dhcpClient()``. See the API
documentation for more information.

Note that this page refers specifically to the DHCP client support built
into Embedded Xinu, which is completely separate from the DHCP support
included in CFE, which is third-party firmware.

External links
--------------

-  `RFC 2131: Dynamic Host Configuration
   Protocol <http://www.ietf.org/rfc/rfc2131.txt>`__

