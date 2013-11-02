Installing OpenWRT
==================

What is OpenWRT?
----------------

OpenWRT is essentially a Linux distribution for embedded systems,
specifically routers. It has an incredibly modular structure which
allows it to build easily for dozens of different devices and makes
package selection easy. It also makes browsing around its source
relatively difficult. When you first download a copy of "White Russian",
the stable branch of OpenWRT, you don't have a Linux kernel, or even a
toolchain, but you have its unique build system, which is everything you
need to build a firmware image. As far as we can tell the build process
follows these steps:

-  Download the correct toolchain
-  Build the toolchain
-  Download a linux kernel
-  Download selected packages
-  Use the toolchain to build the kernel / packages
-  Smush everything together into several flavors of executable
   (depending on file system)
-  Also create versions with proper Linksys headers so that they can be
   uploaded though the web interface as "legit" firmware upgrades

So though the precious bounty is not in the original download, once
you complete the build process, the build system leaves the linux
source behind in the **build_mipsel** directory, and the toolchain is
left waiting to be swiped as well. Although we recommend building your
own :ref:`cross compiler <cross_compiler>` which can be more finely
tuned.

The |EX| project has used OpenWRT's linux source as a reference in our
work on implementing XINU for MIPS.

Installing OpenWRT
------------------

This is a quick overview of the very easy process of installing the
OpenWRT open source firmware into a LinkSys WRT54GL. Several much more
detailed sources of documentation exist on this [#installing1]_
[#installing2]_, but we include
bare essentials here for simplicity's sake and also because the bulk of
OpenWRT does not interest us--it only provides us with a working
open-source implementation of an embedded operating system on the
router.

OpenWRT provides a mature environment for exploring your router
hardware, but is not required to run XINU.

Before Starting
~~~~~~~~~~~~~~~

Get the latest OpenWRT binary from
http://downloads.openwrt.org/whiterussian/newest/bin/. The correct file
is ``openwrt-wrt54g-squashfs.bin``

Steps to Install OpenWRT
~~~~~~~~~~~~~~~~~~~~~~~~

#. Connect to the LinkSys web interface by visiting its address (default
   192.168.1.1) in a web browser.
#. Upload the OpenWRT file as a "firmware update".
#. Watch as your router magically[#magic]_ transforms into an
   OpenWRT-running box.

What to do next?
~~~~~~~~~~~~~~~~

First it is a good idea to play around with the OpenWRT installation and
get a feel for its web interface. You can also login via ssh to the
router and poke around its directory structure.

The next big step towards a custom operating system on the router is
covered in the next HOWTO in which we will :doc:`modify the Linksys
hardware <HOWTO-Modify-the-Linksys-Hardware>`.

Related Links
-------------

-  http://www.openwrt.org - OpenWRT home page
-  http://wiki.openwrt.org/OpenWrtDocs/Hardware/Linksys/WRT54GL -
   information on WRT54GL router

Notes
-----

.. [#installing1] http://wiki.openwrt.org/OpenWrtDocs/Installing
.. [#installing2] http://wiki.openwrt.org/InstallingWrt54gl
.. [#magic] https://en.wikipedia.org/wiki/Magic_(paranormal)
