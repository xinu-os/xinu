Flashing Firmware
=================

Normally, we `dynamically load
Xinu <HOWTO:Deploy_Xinu#Booting_XINU_on_your_Backend>`__ from an elf
image over the network. In some cases, you may want your kernel to
persist when you power-cycle the platform. For this, we write the kernel
to flash memory instead of just loading the kernel into RAM at boot
time. You may also want to replace the default firmware with another
version of embedded Linux or perhaps restore the factory firmware should
your router become corrupted somehow. Below we detail a variety of
methods which should accomplish any of the aforementioned tasks.

Kernel Image Formats
--------------------

You will commonly see firmware images intended for flashing in two
formats: BIN and TRX. For the precise differences, see this `OpenWrt
article <http://wiki.openwrt.org/doc/techref/header>`__ and our `TRX
header <TRX header>`__ page. Essentially, a TRX header provides the
loader with some basic layout information and the BIN format extends
this by adding some platform specific tags. Some of the flashing
mechanisms check for the BIN information so you don't accidentally flash
a kernel that won't run on a platform it wasn't designed for. In these
cases, you want to find a BIN image specific to your platform.
Otherwise, some methods trust you to pick the correct TRX image and
don't require the extra header. When using these methods, do ensure your
kernel is compatible before proceeding.

Web GUI
-------

Linksys Factory Firmware
~~~~~~~~~~~~~~~~~~~~~~~~

dd-wrt
~~~~~~

OpenWrt
~~~~~~~

TFTP
----

CFE Tricks
~~~~~~~~~~

Bootloader with TFTP
--------------------

CFE
~~~

U-Boot
~~~~~~

Linux MTD
---------

Xinu
----

TODO: Figure out if this works now that we are confident we can revive
routers. Intel style flash only???
