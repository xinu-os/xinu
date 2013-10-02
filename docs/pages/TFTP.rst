TFTP
====

**TFTP** (**Trivial File Transfer Protocol**) support has been added to
`Embedded Xinu <Embedded Xinu>`__. Currently, only client support---
that is, downloading files with TFTP Get requests--- is supported. The
code is located in . The API includes the header declaring ``tftpGet()``
and ``tftpGetIntoBuffer()``. See the API documentation for more
information.

Note that this page refers specifically to the TFTP client support built
into `Embedded Xinu <Embedded Xinu>`__, which is completely separate
from the TFTP support included in CFE, which is third-party firmware.

External Links
--------------

-  `RFC 1350: The TFTP Protocol (Revision
   2) <http://tools.ietf.org/html/rfc1350>`__

