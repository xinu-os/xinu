Flash memory
============

.. note::

   This page appears to have been written with :doc:`WRT54GL` routers
   in mind and may or may not be applicable to other platforms.

Like other memory mapped hardware devices on the MIPS platform,
**Flash memory** has an address range in KSEG1. This means that the
memory is **unmapped** and **uncached**. An interesting, and
important, piece of information is that all 4 megabytes of Flash
memory is mapped 1-1 into the address range between ``0xBC00 0000``
and ``0xBC3F FFFF``. This allows for random read-access without using
an interface to load the data a fixed amount of registers.

Data Locations
--------------

Stored on Flash memory are several important parts to make a backend
work properly, some of these data points are listed below.

-  ``0xBC00 1000`` has backup NVRAM settings. If the "proper" settings
   become corrupt, CFE will replace the proper settings with these.
-  ``0xBC00 1E00`` holds the "true" MAC address of device. During CFE
   boot, this is the address that will be used. Once a full kernel has
   been loaded the MAC address may be different.
-  ``0xBC00 1F00`` holds the current CFE boot version (should be "v3.7"
   for :doc:`WRT54GLs <WRT54GL>`).
-  ``0xBC00 2000`` is the beginning of CFE code.
-  ``0xBC03 F400`` is a unique device ID (should match the NVRAM setting
   for ``eou_device_id``).
-  ``0xBC03 F408`` is a unique private key for device (should match the
   NVRAM setting for ``eou_private_key``).
-  ``0xBC03 F508`` is a unique public key for device (should match the
   NVRAM setting for ``eou_public_key``).
-  ``0xBC04 0000`` is the beginning of the operating system kernel
   (|EX| or some Linux variant). Typically, this will be a gzipped
   version of the raw kernel code prefixed with a
   :doc:`TRX-Header`.
-  ``0xBC3F 8000`` is the location of proper :ref:`NVRAM settings
   <nvram>`.

This is not a comprehensive list of memory locations within Flash
memory, but a guide of where some values may be stored when trying to
interface with a new system.

Writing Flash
-------------

Writing data to Flash memory is not simply a matter of writing data to
the memory address. For the WRT54GL backends the common NOR type of
Flash memory is used. As with all Flash memories there are certain
properties that must be followed when storing data.

An important property of NOR based Flash memory is that each bit on
Flash can only be changed from a ``1`` to a ``0`` and not the other way
around. So if a byte has the pattern ``1001 1101`` only the high bits
can be written. This presents an interesting challenge for efficient
file system structures. Once a bit has been set to ``0`` and the
operating system wishes to reset the bit to ``1`` a special erase
command must be sent to the device. Because of this Flash memory is
broken down into several distinct segments called *erase blocks*. These
erase blocks can vary in size between Flash memory chips and even within
the same chip. After sending the command to erase an erase block, the
entire erase block will be set to ``1``\ s, allowing any data to be
written. The specific method of erasing and writing data depends on the
manufacturer of the underlying hardware. In general it is a matter of
writing a sequence of values to a certain location to prepare the
device, then writing the new data to the correct position. These
operations are more detailed on the :doc:`Flash-Driver` page
under :ref:`physical layer <flash_driver_physical_layer>`.

Common Flash Interface
----------------------

Luckily for software authors, the manufacturers of Flash memory have
developed a standard for discovering information about a Flash device,
called the Common Flash Interface (CFI). By implementing a CFI query
routine, the operating system can discover what command set the chip
implements (Intel, AMD, and Mitsubishi all have a standard command set
and extended command set). Other information that can be queried is
voltages, timeouts for writing and erasing, access mode (word or byte),
device size (as :math:`2^n`), and information about up-to four erase
block regions. The information about each erase block region will
consist of the size of an erase block in the region and the number of
equally sized erase blocks that exist in the regions.

.. _nvram:

NVRAM
-----

While technically a misnomer, NVRAM (non-volatile random access memory)
refers specifically to the platform settings stored across power cycles
of the device. These settings will always begin 8 pages (32 kilobytes)
away from the end of Flash memory. In the case of the WRT54GL, this
means NVRAM settings are stored beginning at ``0xBC3F 8000``. At this
point there is a 20 byte header which is laid out as follows:

| `` 0                   1                   2                   3   ``
| `` 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 ``
| ``+---------------------------------------------------------------+``
| ``|                     magic number ('FLSH')                     |``
| ``+---------------------------------------------------------------+``
| ``|                length (header size + variables)               |``
| ``+---------------+---------------+-------------------------------+``
| ``|      CRC      |    version    |         SDRAM Init (?)        |``
| ``+---------------+---------------+-------------------------------+``
| ``|       SDRAM config (?)        |       SDRAM refresh (?)       |``
| ``+-------------------------------+-------------------------------+``
| ``|                        NCDL value (?)                         |``
| ``+---------------------------------------------------------------+``

Several of the values are not used by Embedded Xinu as the values
represent something that is not fully understood (all the SDRAM values
and the NCDL value).

Immediately after the header begins the NVRAM settings as NULL delimited
``name=value`` tuples stored as plain text. It is possible that after
the final tuple the settings are NULL character padded to the nearest 4
byte word.
