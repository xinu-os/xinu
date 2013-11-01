OpenWRT
=======

OpenWRT is essentially a Linux distribution for embedded systems,
specifically routers. It has an incredibly modular structure which
allows it to build easily for dozens of different devices and makes
package selection easy. It also makes browsing around its source
relatively difficult. When you first download a copy of "White Russian",
the stable branch of OpenWRT, you don't have a linux kernel, or even a
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

So though the precious bounty is not in the original download, once you
complete the build process, the build system leaves the linux source
behind in the **build\_mipsel** directory, and the toolchain is left
waiting to be swiped as well. Although we recommend building your own
`cross compiler <HOWTO:Build XINU>`__ which can be more finely tuned.

The `Embedded XINU <Main Page>`__ project has used OpenWRT's linux
source as a reference in our work on implementing XINU for MIPS.

Related Links
-------------

-  http://www.openwrt.org - OpenWRT home page
-  http://wiki.openwrt.org/OpenWrtDocs/Hardware/Linksys/WRT54GL -
   information on our primary router

