TFTP
====

**TFTP** (**Trivial File Transfer Protocol**) support has been added
to XINU as part of its :doc:`networking subsystem <index>`.
Currently, only client support has been implemented, and even then
only TFTP Get requests are supported.  (That is, downloading files is
supported but uploading files is not.)  The API is declared in
:source:`include/tftp.h` and includes :source:`tftpGet()
<network/tftp/tftpGet.c>` and :source:`tftpGetIntoBuffer()
<network/tftp/tftpGetIntoBuffer.c>`.  See the API documentation for
more information.

Note that this page refers specifically to the TFTP client support
built into XINU, which is completely separate from the TFTP support
included in :doc:`CFE </mips/Common-Firmware-Environment>` on the
various MIPS-based routers, which is third-party firmware.

:source:`include/tftp.h` also includes several compile-time parameters
and a :doc:`trace macro </development/Trace>` that can be enabled for
the TFTP code.

Resources
---------

- :wikipedia:`Trivial File Transfer Protocol - Wikipedia <Trivial File Transfer Protocol>`
- :rfc:`1350`
