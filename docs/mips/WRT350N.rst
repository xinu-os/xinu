WRT350N
=======

`thumb\|150px\|left\|The Satellite <Image:WRT350N.png>`__

Caveats out of the box
----------------------

This router claims to use CFE version 1.0.37 before booting into linux.
Although, it appears slightly different than the version on the lab's
WRT54GL's. Most noticeably, CFE does not recognize the ethernet hardware
when first booting. Recognizing NVRAM settings and communicating with
the network is reserved for after linux is up. This made the device
unable to load the XINU code using the "boot -elf..." command. A tip
from the OpenWRT forums gives a little insight: `WRT350N
bricked <http://forum.openwrt.org/viewtopic.php?pid=47784>`__

A network cable was run from a front-end machine capable of
communicating with the router at its default address of 192.168.1.1.
From there, a blank image was tftp'd (using put) to the router during
booting. The inability to verify this image as valid causes the device
to drop to a CFE prompt--this time with the network communicating. At
this point, one can use the standard procedure of ifconfig and boot
commands as per WRT54GL protocol. Strangely, just as this procedure grew
tedious, the WRT350N CFE prompt changed its disposition, and began
communicating with the server as hoped. Apparently, beating the device
with empty images through tftp does the trick.

It should also be noted that you must make sure the router has
boot\_wait=on for this to work.
