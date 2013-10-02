Processor
=========

Coprocessor 0 contains a wealth of information which is required of any
MIPS CPU. Registers of note are processor identification (register 15)
and two configuration registers (register 16, select 0 and select 1).
Below is what has been gleaned from the processor on the WRT54GL and the
WRT54G (differences will be noted).

Processor Identification
------------------------

This register will contain information about what company manufactured
the CPU, what options are installed, the implementation of the processor
and the revision of the processor.

| ``0x00029008 (WRT54GL)``
| ``0x00029029 (WRT54G)``

The identification is split into 4 8-byte chunks in the following order
(starting with bit 31): manufacturer options, manufacturer
identification, processor implementation, and revision. For both the
WRT54GL and the WRT454G there are no manufacturer options. A
manufacturer identification of 0x02 is reserved for broadcom processors.
Of most importance is the processor implementation, which for both units
is 0x90, which makes the processor a MIPS4KEc (MIPS32 R2 compliant). The
last field, revision, is the only difference and should be unimportant
for XINU.

Configuration
-------------

Configuration for MIPS processors is stored on coprocessor 0, and
contains important information about what the processor supports, how
memory works, and what is implemented on the processor. Configuration is
always in register 16, but it is possible to access different data by
selecting what configuartion data we are looking for (select 0 for
CONFIG1, select 1 for CONFIG2).

Config, select 0
~~~~~~~~~~~~~~~~

Both the WRT54GL and WRT54G contain the same value for config registers
``0x80000083``. The significant information is listed below:

-  bit 31 - (1) CP0\_CONFIG1 exisits
-  bit 15 - (0) Little Endian
-  bit 14:13 - (0) MIPS 32 standard
-  bit 12:10 - (0) Revision 1
-  bit 9:7 - (1) MIPS 32/64 Compliant TLB
-  bit 3 - (0) I-cache is not indexed or tagged with virtual addresses
-  bit 2:0 - (3) kseg0 coherency algorithm (cacheable)

Config, select 1
~~~~~~~~~~~~~~~~

The second configuration register on the MIPS processor reports slightly
different data between the WRT54GL and the WRT54G. Data from the WRT54GL
will be introduced first.

``0x3ed94c82 (WRT54GL)``

-  bit 30:25 - (31) 2\ :sup:`5` entries in TLB (31+1)
-  bit 24:22 - (3) I-cache number of index positions 64\*2\ :sup:`3` =
   512-bytes
-  bit 21:19 - (3) I-cache line size 2\*2\ :sup:`3` = 16-bytes
-  bit 18:16 - (1) I-cache associativity (A+1) = 2-way set associative
-  bit 15:13 - (2) D-cache number of index positions 64\*2\ :sup:`2` =
   256-bytes
-  bit 12:10 - (3) D-cache line size 2\*2\ :sup:`3` = 16-bytes
-  bit 9:7 - (1) D-cache associativity (A+1) = 2-way set associative
-  bit 6 - (0) no CP2
-  bit 5 - (0) no MDMX ASE implementation
-  bit 4 - (0) no performance counter
-  bit 3 - (0) no watchpoint register
-  bit 2 - (0) no MIPS16e instruction set available
-  bit 1 - (1) EJTAG debug unit IS available
-  bit 0 - (0) no floating point unit attached

This gives the following data: I-cache has 512 sets/way, 16 bytes/line,
and is 2-way set-associative (16 KByte I-cache) and D-cache has 256
sets/way, 16 bytes/line, and is 2-way set-associative (8 KByte D-cache).

``0x3e9b6c86 (WRT54G)``

-  bit 30:25 - (31) 2\ :sup:`5` entries in TLB (31+1, same as WRT54GL)
-  bit 24:22 - (2) I-cache number of index positions 64\*2\ :sup:`2` =
   256-bytes
-  bit 21:19 - (3) I-cache line size 2\*2\ :sup:`3` = 16-bytes
-  bit 18:16 - (3) I-cache associativity (A+1) = 2-way set associative
-  bit 15:13 - (3) D-cache number of index positions 64\*2\ :sup:`3` =
   512-bytes
-  bit 12:10 - (3) D-cache line size 2\*2\ :sup:`3` = 16-bytes
-  bit 9:7 - (1) D-cache associativity (A+1) = 2-way set associative
-  bit 6 - (0) no CP2
-  bit 5 - (0) no MDMX ASE implementation
-  bit 4 - (0) no performance counter
-  bit 3 - (0) no watchpoint register
-  bit 2 - (1) MIPS16e instruction set IS available
-  bit 1 - (1) EJTAG debug unit IS available
-  bit 0 - (0) no floating point unit attached

Again, giving the following data (which does differ from the WRT54GL):
I-cache has 256 sets/way, 16 bytes/line, and is 2-way set-associative (8
KByte I-cache) and D-cache has 512 sets/way, 16 bytes/line, and is 2-way
set-associative (16 KByte D-cache)
