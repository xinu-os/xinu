Router Recovery
===============

So you've created yourself an expensive paperweight...

Not to worry! If you aren't breaking things, then chances are you aren't
making hardcore progress. This page serves as a knowledge pool for
methods to revive routers that are corrupted or otherwise considered
non-functional. The information below is mostly specific to the WRT54GL
as it is our most popular and well understood platform at this time. The
process is somewhat similar for other models/platforms; however, some of
the utilities are limited to specific platforms and commands vary
slightly between bootloaders (ex.
`http://sourceforge.net/projects/u-boot/
U-Boot <http://sourceforge.net/projects/u-boot/ U-Boot>`__ vs CFE).

Sometimes the router won't boot because of a corrupted NVRAM variable
and a simple factory reset will resolve the problem. Chances are if
you've sought out this page you are in much deeper and probably need a
more serious TFTP or `JTAG <EJTAG>`__ recovery. We'll start with the
simple solutions and work our way to the more intense recovery methods.

Before You Begin
----------------

If you haven't already, `backup your router's
configuration <HOWTO:Backup_your_router>`__. Hopefully you did this
earlier so you can restore to a "known good" working state. If you
didn't, it is still a good idea to do that now; it can always get worse.
You'll also want to grab yourself a copy of some reliable firmware. The
default firmware that shipped with your router is a good place to start
(generally available from the manufacturer's website). Otherwise, a
stable release of your favorite embedded Linux distribution is a good
alternative.

Factory Reset
-------------

Don't get your hopes up for this one, but sometimes
`http://en.wikipedia.org/wiki/Occam's\_razor Occam's
razor <http://en.wikipedia.org/wiki/Occam's_razor Occam's razor>`__
applies to router recovery. To do a factory reset, hold down the reset
switch for about 10 seconds while the unit is powered on, then unplug
it. Let it rest for a little while then power it back up. If this isn't
working for you, try the dd-wrt
`http://www.dd-wrt.com/wiki/index.php/Hard\_reset\_or\_30/30/30
30/30/30 <http://www.dd-wrt.com/wiki/index.php/Hard_reset_or_30/30/30 30/30/30>`__
reset method.

There's another option if you have access to a serial console, but your
router isn't necessarily readily accessible (say locked in a rack
somewhere with a pool of backends). Access the CFE as you normally
would. Then, issue the command ``<pre>CFE> nvram erase</pre>`` and then
reboot. Some models do not properly reinitialize their NVRAM variables
automatically, so be careful with this method. The WRT54GL does recover
them conveniently from a separate stored location in flash.

If you haven't caught on by now, these methods erase any custom settings
that were stored in NVRAM. Don't forget to re-configure and commit your
network settings, if applicable.

Serial Console
--------------

Now is a good time to check that your serial console is up and running.
If you don't see any text coming across the serial link, then you should
double check your transceiver is working properly. Swap with a working
one or at least try the one in question on another working router, if
you have one available. If you can access the web interface (assuming
the OS in flash has one) at either the default IP address or the one you
configured, then your flash image is probably fine--fix your serial
console. It is important that you are confident your serial console if
functional; henceforth we'll be looking for output on the serial port as
a metric for whether each recovery method is successful or not.

If you can see text and access the CFE but your router won't boot, try
re-flashing the firmware with `TFTP <Flashing Firmware#TFTP>`__.

If you are confident the serial interface hardware is working properly
but your router appears dead, then proceed to the next section.

JTAG
----

In a nutshell, JTAG is an external interface that allows external
control of an SoC and its memory. You can read more about this on our
EJTAG page. JTAG allows us to recover routers that are completely
unresponsive (aka debricking). Before continuing, you'll need a JTAG
cable (active or passive will do) and a header soldered to the JTAG
connector on your router. See the references below for some suggestions
on this bit.

Software
~~~~~~~~

For WRT54GL recovery, the popular programs are the original
HairyDairyMaid utility and a port called TJTAG. If you purchase a
commercial cable, it may come with a hardware specific recovery tool.
This guide focuses on TJTAG. Download a copy of the source (linked
below) and compile as usual.

If you built your own cable, chances are it uses a legacy printer style
parallel port interface. You'll need adequate permissions to use this
device in order to run the JTAG software. In \*nix style systems usually
means the user you execute TJTAG as must be a member of the ``lp``
group.

The result of the ``groups`` command should look something like this
before you continue: ``<pre>
user@host:~$ groups
user lp dialout
</pre>`` Membership of the ``dialout`` group isn't strictly necessary;
however, you will need this as well if you want to use a local serial
console.

Establish a Connection
~~~~~~~~~~~~~~~~~~~~~~

Now we must verify that TJTAG can properly connect to the JTAG interface
on the router. Connect the JTAG cable to both your PC and the router but
leave the power disconnected. On most routers you will be fighting the
`watchdog timer <http://en.wikipedia.org/wiki/Watchdog_timer>`__ so it
is a good idea to type out whatever command you want to execute (without
hitting ``enter``), then provide power, and finally quickly hit
``enter`` as soon as the router LEDs light up.

Active vs Passive
^^^^^^^^^^^^^^^^^

If you built the active buffered cable you need to add the ``/wiggler``
option to all of your TJTAG commands. The passive unbuffered cable does
not require this option and you should leave it off when using this type
of cable. If you anticipate needing to revive routers often, an active
cable is surely worth the additional investment in time and parts so you
aren't restricted to working within 6 inches of your parallel port.
Otherwise, the unbuffered cable works great provided you can manage the
logistics of the restricted cable length.

TJTAG Options
^^^^^^^^^^^^^

The next trick is to find the magical combination of optional TJTAG
parameters which makes your router happy. Even within a single
make/model this seems to vary greatly--most likely because of various
flash chip manufacturers. For starters, we'll use the ``-probeonly``
option to guess and check which options will work before modifying the
contents of flash. Usually something like
``<pre>user@host:tjtag$ ./tjtag -probeonly /wiggler /noemw /noreset</pre>``
will do the trick. If you are not getting the desired output (see
below), try experimenting with the DMA, break, and reset switches. Once
you've mastered the combinatorics game, you can move onto read/write
operations.

Successful Output
^^^^^^^^^^^^^^^^^

When you've got the right combinations of parameters, you should see an
output like this: ``<pre>
==============================================
 EJTAG Debrick Utility v3.0.1 Tornado-MOD 
==============================================

Probing bus ... Done

Instruction Length set to 8

CPU Chip ID: 00000101001101010010000101111111 (0535217F)
*** Found a Broadcom BCM5352 Rev 1 CPU chip ***

    - EJTAG IMPCODE ....... : 00000000100000000000100100000100 (00800904)
    - EJTAG Version ....... : 1 or 2.0
    - EJTAG DMA Support ... : Yes
    - EJTAG Implementation flags: R4k MIPS32

Issuing Processor / Peripheral Reset ... Skipped
Enabling Memory Writes ... Skipped
Halting Processor ... <Processor Entered Debug Mode!> ... Done
Clearing Watchdog ... Done

Probing Flash at (Flash Window: 0x1fc00000) ... 
Done

Flash Vendor ID: 00000000000000000000000011101100 (000000EC)
Flash Device ID: 00000000000000000010001010100010 (000022A2)
*** Found a K8D3216UBC  2Mx16 BotB     (4MB) Flash Chip ***

    - Flash Chip Window Start .... : 1fc00000
    - Flash Chip Window Length ... : 00400000
    - Selected Area Start ........ : 00000000
    - Selected Area Length ....... : 00000000



 *** REQUESTED OPERATION IS COMPLETE ***
</pre>`` The last line is important. Don't move on until you get this
response.

Troubleshooting
^^^^^^^^^^^^^^^

If you see something like this: ``<pre>
==============================================
 EJTAG Debrick Utility v3.0.1 Tornado-MOD 
==============================================

Probing bus ... Done

Instruction Length set to 8

CPU Chip ID: 00000101001101010010000101111111 (0535217F)
*** Found a Broadcom BCM5352 Rev 1 CPU chip ***

    - EJTAG IMPCODE ....... : 00000000100000000000100100000100 (00800904)
    - EJTAG Version ....... : 1 or 2.0
    - EJTAG DMA Support ... : Yes
    - EJTAG Implementation flags: R4k MIPS32

Issuing Processor / Peripheral Reset ... Done
</pre>`` You probably don't have the correct combination of options for
your router. Play with the different switches available before
attempting to read/write from flash.

If you see something like this: ``<pre>
==============================================
 EJTAG Debrick Utility v3.0.1 Tornado-MOD 
==============================================

Probing bus ... Done

Instruction Length set to 5

CPU Chip ID: 11111111111111111111111111111111 (FFFFFFFF)
*** Unknown or NO CPU Chip ID Detected ***

*** Possible Causes:
    1) Device is not Connected.
    2) Device is not Powered On.
    3) Improper JTAG Cable.
    4) Unrecognized CPU Chip ID.
</pre>`` Aside from what the output mentions already check that

-  the header is soldered properly
-  tjtag has permission to use the parallel port
-  you didn't forget the ``/wiggler`` switch (active cable only).

General Advice for Read/Write Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the next few steps you'll attempt to correct some issues in flash
memory that might prevent the router from booting correctly. Ideally
when you deal with flash, it is best not to interrupt the process before
it finishes on its own. That is why it is important to get the TJTAG
options correct with the ``-probeonly`` option. If an operation hangs
while attempting to read/write, don't panic. It is probably in your best
interests not to buy any lottery tickets tonight, but most likely all is
not lost. Be patient--be sure you've given it ample time to complete. If
it doesn't seem to be making any progress then you probably need to
reset the router. Various sources on the Internet have different
opinions on the best way to reset. Nevertheless, we've found that
disconnecting the power first and then canceling the operation (via
CTRL+C) works the best. Lastly, try the operation again (double check
your parameters).

Erase NVRAM
~~~~~~~~~~~

Step 1. As before, it's a good idea to ensure NVRAM has been wiped out
and isn't harboring corrupt variables. Use the same TJTAG options that
got you a successful completion when trying to probe the device.

Example:
``<pre>user@host:tjtag$ ./tjtag -erase:nvram /wiggler /noemw /noreset</pre>``
When the device reboots, it should reinitialize the correct NVRAM
settings from the backup location within the CFE. If you now have a
serial console, success, you're good to go. Don't forget to reconfigure
any custom NVRAM settings. If that didn't work, read on.

Erase the Kernel
~~~~~~~~~~~~~~~~

Step 2. For whatever reason, it is possible for a corrupted kernel to
prevent the bootloader from producing any output. Again, use the same
options you figured out during the ``-probeonly`` phase.

Example:
``<pre>user@host:tjtag$ ./tjtag -erase:kernel /wiggler /noemw /noreset</pre>``
After rebooting your router, you should get console output that
indicates the CFE was upset to find there's no kernel to load from
flash. This is normal; you did just erase this region of flash
(hopefully). Now you can proceed to `flashing a new
kernel <Flashing Firmware>`__ or maybe you just want to `load an elf
image <HOWTO:Deploy_Xinu#Booting_XINU_on_your_Backend>`__ over the
network.

Still got a blank serial console? Read on.

CFE Recovery
~~~~~~~~~~~~

Step 3. Your last ditch effort is to replace the bootloader. If your CFE
is corrupted then there's no hope of booting. Luckily, we can flash new
one using TJTAG. The CFE contains a few settings unique to each router.
If you made a backup of your CFE before things went South, you can use
that to restore the router to working order. If you don't have a backup,
you can borrow a copy from another identical router. In the latter case
you must customize the CFE binary a bit before using it on a different
router. In the event you don't have access to a working router of the
same make and model, try searching around on the Internet for a pristine
CFE. There have been a few "CFE collection" projects out there; you
might get lucky.

Transplant Another Router's CFE
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have the original CFE for this exact router, skip ahead to the
next section. Otherwise, start by cloning the CFE of your good router
using `the usual methods <HOWTO:Backup_your_router>`__.

There are a few CFE variables unique to each router. For the WRT54GL,
see our `flash memory <flash memory>`__ page for specific locations.
There should be a unique identifier and a pair of cryptography keys as
well as the device MAC addresses. We're primarily concerned with the MAC
address of the first physical Ethernet interface, but feel free to
update the others as well. For the WRT54GL, 0x1E00 contains the MAC
address used by the CFE at boot time. While you can override this
setting later for your kernel in your local network configuration or
NVRAM, you can't fool the bootloader (well you can, but not
persistently). Especially, if you are running a pseudo-static DHCP
configuration for a pool of backends, you'll get lots of network
conflicts during the boot process unless this is set correctly. For the
WRT54GL, this MAC address should match the one on the bottom sticker.
Other routers have various schemes/offsets for what the address should
be relative to the one printed on the case depending on how many
physical interfaces the unit has and how the manufacturer chose to
allocate the addresses. If you are feeling ambitious, you can update the
default NVRAM settings in the CFE backup location so they are correct
when you do a factory reset. Alternatively, these can always be
corrected later using the NVRAM utilities.

To edit the CFE, fire up your favorite hex editor and tweak each
location as necessary. We happen to like ``shed``, which has a nice
``nano`` like interface. Note that with ``shed`` your changes are
affecting the file directly as you make them. There is no "quit without
saving" option. It is always a good idea to make a copy of the CFE.BIN
file you are planning to edit so you can revert to the original without
having to grab it off the good router again.

Example: ``<pre>
offset    asc hex dec oct bin
00001DFE:     00  000 000 00000000
00001DFF:     00  000 000 00000000
00001E00:  C  43  067 103 01000011
00001E01:  0  30  048 060 00110000
00001E02:  :  3A  058 072 00111010
00001E03:  F  46  070 106 01000110
00001E04:  F  46  070 106 01000110
00001E05:  :  3A  058 072 00111010
00001E06:  E  45  069 105 01000101
00001E07:  E  45  069 105 01000101
00001E08:  :  3A  058 072 00111010
00001E09:  C  43  067 103 01000011
00001E0A:  0  30  048 060 00110000
00001E0B:  :  3A  058 072 00111010
00001E0C:  F  46  070 106 01000110
00001E0D:  F  46  070 106 01000110
00001E0E:  :  3A  058 072 00111010
00001E0F:  E  45  069 105 01000101
00001E10:  E  45  069 105 01000101
00001E11:     00  000 000 00000000
00001E12:     FF  255 377 11111111
00001E13:     FF  255 377 11111111
00001E14:     FF  255 377 11111111
                                                             1E00/40000 (hex) 
SPACE|E edit  S|W|F search  J jump to   T dec/hex   D dump     1|2|4 cursor
X       exit  R|N   repeat  B bin edit  A ext. asc  P preview  `     endian
</pre>``

You should now have a CFE.BIN file with the correct MAC address(es)
ready for flashing! Proceed to the instructions below as if you had a
correct backup all along.

Using the Same Router's Backup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Copy your CFE binary to a file called CFE.BIN in the same directory as
the TJTAG executable. You don't have a choice for what file TJTAG reads
in. When you are ready to write it to the router, once again make sure
you know which TJTAG switches keep your particular router happy.

Example:
``<pre>user@host:tjtag$ ./tjtag -flash:cfe /wiggler /noemw /noreset</pre>``

Now, go get a cup of coffee or take a power nap. Bit-banging a few
hundred kilobytes with a parallel interface can take awhile. You should
see exactly what TJTAG is writing to flash whirring by as well as the
percentage written slowly increasing. Sometimes the watchdog timer
doesn't disable correctly and you have to try this a few times before it
will keep going. If it going to fail usually this happens within the
first few seconds.

A fresh CFE should fix most stubborn routers that aren't physically
damaged, but we make no guarantees. Your mileage may vary. Hopefully you
have your serial console back and you can proceed to restoring a new
kernel. If your router is still unresponsive, unfortunately we have no
more advice for you. It is probably time to invest in a new one.

External References
-------------------

-  `OpenWrt JTAG
   Reference <http://wiki.openwrt.org/doc/hardware/port.jtag>`__
   (includes various DIY cable designs and links to commercial products)
-  `OpenWrt Utilities <http://downloads.openwrt.org/utils/>`__
   (HairyDairyMaid Download)
-  `TJTAG
   Download <http://www.tiaowiki.com/download//category.php?id=8>`__
-  `TIAO Wiki Debrick
   Guide <http://www.tiaowiki.com/w/Debrick_Routers_Using_JTAG_Cable>`__
-  `Shed <http://sourceforge.net/projects/shed/>`__ (Simple Hex Editor)

