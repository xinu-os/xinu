WRT54G
======

.. raw:: mediawiki

   {{Cleanup}}

Transceiver from GL causes dropped characters; using
`Linksys <http://www.linksys.com>`__ transceiver

-  [STRIKEOUT:possibly solved by "tighter" transceiver construction]
-  construction of transceiver using Analog Devices ADM202JN using .22uF
   capacitors in charge pump (C1 + C2)
-  possibility of strapping additional capacitance across internal
   condensers of Maxim transceiver

   -  pin8:C1+ ; pin13:C1- using 0.1uF or greater capacitor
   -  pin11:C2+ ; pin10:C2- using 0.1uF or greater capacitor

loading CFE
-----------

VxWorks will flash over itself with CFE -
http://wiki.openwrt.org/OpenWrtDocs/Hardware/Linksys/WRT54G

Attempted to load micro openWRT kernel and CFE over VxWorks

-  upload failed - incompatible browser?
-  despite successful re-load of original firmware, bootloader refuses
   to find image on flash
-  possible solutions are loading fresh VxWorks from network or
   reloading flash through EJTAG port

loading XINU
------------

Control-C interrupts VxWorks loader, as it does in CFE. VxWorks has an
advantage over CFE in that it will remember bootloader settings for tftp
host after powercycle. This removes the need for special xinu-console
script to talk to CFE. Just run xinu-console, download compiled boot
image, and powercycle.

Settings for VxWorks bootloader:

-  boot device: vl0
-  host name: morbius
-  file name: {backend name}.boot
-  inet on ethernet: 192.168.6.{assigned by morbius}
-  host inet: 192.168.6.10
-  flags 0x88 --tftp and quickboot

Changes to current XINU revision for 'G' compatibility: +

-  loader/start.S

   -  change max physical address from 0x81000000 to 0x80800000 (cut in
      half)
   -  removal of cfe\_api.c from loader, remove references in start.S $
   -  replaced CFE cache flushing with fresh cache initialization
      function
   -  altered mips.h for macros in loader cache init function

-  system/xdone.c - remove CFE\_exit(), add new halt() function
-  uart/uartInit.c

   -  remove **ucptr->uart\_dll = 0x0B;**
   -  remove **ucptr->uart\_dlm = 0x00;**
   -  If one queries the hardware, values to replace **0B** and **00**
      can be identified, **0E** and **00**

+ Loader now reads CPU revision number from CP0 PRId register. This
automatically sets max physical address and UART baud rate divisor based
on router model.

The CFE cache initialization has been replaced in XINU v1.0 for WRT54G
compatibility.

See also
--------

-  WRT54GL
-  `WRT54G Wiki <wikipedia:WRT54G>`__

