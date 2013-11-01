TRX Header
==========

TRX is the format used to store kernel images in `Flash
memory <Flash memory>`__ for CFE based (and possibly others) routers.
Unfortunately, not much is known about the format or what limitations
and rules exist. Currently, `Embedded Xinu <Embedded Xinu>`__ uses a
simple utility from the OpenWRT repository to build TRX images. However,
the quality of our TRX images seems to vary between revisions of the
operating system. We do not yet know why. Possible reasons include:

-  alignment problems,
-  compression/extraction problems,
-  incorrectly configuration at startup (differences between TFTP
   booting and booting from Flash), or a
-  combination of above and some unknown problem.

What we do know is the format of the 28 byte TRX header, it is as
follows:

| `` 0                   1                   2                   3   ``
| `` 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 ``
| ``+---------------------------------------------------------------+``
| ``|                     magic number ('HDR0')                     |``
| ``+---------------------------------------------------------------+``
| ``|                  length (header size + data)                  |``
| ``+---------------+---------------+-------------------------------+``
| ``|                       32-bit CRC value                        |``
| ``+---------------+---------------+-------------------------------+``
| ``|           TRX flags           |          TRX version          |``
| ``+-------------------------------+-------------------------------+``
| ``|                      Partition offset[0]                      |``
| ``+---------------------------------------------------------------+``
| ``|                      Partition offset[1]                      |``
| ``+---------------------------------------------------------------+``
| ``|                      Partition offset[2]                      |``
| ``+---------------------------------------------------------------+``

After the TRX header, the data section begins.

Code Pattern
============

It is common to see an additional 32 byte header prepended to the TRX
image as a way to tag certain firmware images specific to a particular
hardware platform. The first 4 bytes correspond to an ASCII "Code
Pattern" that makes this identification. For example, the WRT54G series
routers (including the WRT54GL) contain the characters ``W54G`` as the
first field in this additional header. Some methods of writing to flash,
such as the Linksys Web GUI and TFTP, check for this pattern before
actually writing the firmware. The flash process will error out when
attempting to flash a router without the correct code pattern using
these methods. When the code pattern header is present, filenames
traditionally have the ``.bin`` extension instead of ``.trx``.

Our TRX image creation process for Xinu uses the
`https://dev.openwrt.org/browser/trunk/tools/firmware-utils/src/addpattern.c
addpattern <https://dev.openwrt.org/browser/trunk/tools/firmware-utils/src/addpattern.c addpattern>`__
utility from OpenWRT to add the correct pattern to our TRX file. This
utility also supports additional fields in the code pattern header:
dates, versions, board ID, and flags.
