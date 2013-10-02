Flash Driver
============

`Embedded Xinu <Embedded Xinu>`__ uses a multi-layered approach to
dealing with `Flash memory <Flash memory>`__. This allows the
presentation of a simple and consistent interface to user programs,
while handling the more complicated hardware interface underneath.

High level API
--------------

Like other drivers in Embedded Xinu, the Flash driver provides user
level calls to ``open()``, ``close()``, ``read()``, ``write()``, and
``control()``. In order to begin using a device the user must ``open()``
it, this initializes the logical layer and sets up structures for use.
The complimentary ``close()`` function will clear those structures and
write any cached data to the underlying Flash hardware.

``control()`` provides several functions for getting information and
performing operations on the driver. The control functions are presented
below.

-  ``FLASH_BLOCK_SIZE`` returns the size of logical blocks for the Flash
   device.
-  ``FLASH_N_BLOCKS`` returns the number of block available on the Flash
   device.
-  ``FLASH_SYNC`` forces a synchronization from cached data onto Flash
   memory, the two forms are:

   -  ``FLASH_BLOCK`` synchronizes a specific erase block, and
   -  ``FLASH_LOGBLOCK`` synchronizes a logical block of data.

``read()`` and ``write()``
~~~~~~~~~~~~~~~~~~~~~~~~~~

The Flash device takes three slightly different parameters for
``read()`` and ``write()`` when compared to other devices in Embedded
Xinu. Normal devices will take the device identifier, a buffer, and the
size of the buffer. Since the Flash device uses fixed size logical
blocks, it is assumed that the buffer will be the size of a single
logical block. Therefore, the Flash driver API for ``read()`` and
``write()`` is:

.. code:: c

    read(int device_id, char *buffer, uint block_number);
    write(int device_id, char *buffer, uint block_number);

Logical Layer
-------------

Normal block-oriented devices present a consistent view of data storage
with each block being a small fixed size ranging from 512 bytes to 4,096
bytes. Flash memory does not act like normal block-oriented devices
though. The underlying hardware is separated into erase block regions of
which there can be four. Each erase block region can hold a number of
erase blocks of a fixed size. These fixed sizes can be any size that is
a power of two. For example, on the WRT54GL platform, Flash is separated
into two erase block regions, one with 8 - 8 KB erase blocks and the
other with 63 - 64 KB erase blocks.

To avoid the user level programmer from having to deal with this
inconsistent view of erase blocks, the logical layer of the Flash driver
splits all of Flash memory into uniformly sized logical blocks (at
present this is 512 byte blocks). When a call to ``read()`` occurs, the
logical layer will determine what erase block the logical block maps to,
determine if the erase block has already been loaded into RAM and return
a pointer to the cached data. Similarly, a call to ``write()`` will
perform a mapping from logical to erase block and write the data to the
cached memory. If too many erase blocks are stored in RAM, the logical
layer will evict a block and if it has been modified since the read it
will write it back to Flash memory.

Physical Layer
--------------

Once the logical to erase block mapping has occurred, the logical layer
can pass the erase block stored in RAM to the physical layer to perform
the low level hardware operations. At this layer, the software only
deals in erase block units and uses manufacturer specific code.
Currently, Embedded Xinu fully supports the Intel Standard Command Set
(SCS) and the AMD/Samsung SCS is a work in progress.

Largely, the routines to handle the hardware follow similar concepts.
When a non-read request is made to the physical layer the software steps
through a series of operations to change an erase block from read-mode
to program or erase mode. When this happens, the software is able to
safely modify the data.

An interesting property of Flash memory is that certain devices allow
program and erase operations to be suspended, so it may be possible to
spin the physical layer off as a separate pre-emptible process.
Unfortunately, while the Intel SCS supports suspend/resume operations
the AMD/Samsung SCS does not, so this would lead to compatibility issues
if implemented.

NVRAM
-----

`NVRAM settings <Flash memory#NVRAM>`__ are stored in Flash memory and
take advantage of both the logical and physical layers of the Flash
driver. When NVRAM is first initialized, the Flash driver determines
what logical block the settings begin in and then begins reading and
storing the settings into RAM. Each tuple is indexed into a hash table
and allocated a piece of memory to store the data in. When a setting is
changed, the original value is released from memory and the new value is
added. If a value is requested, the driver will simply find the storage
location and return the pointer to the data.

When the updated settings are committed to Flash, the NVRAM driver will
discover the logical block to erase block mapping, create a complete
erase block with the new settings, cause a write in the physical layer,
and finally force a synchronization to commit the settings to Flash.
