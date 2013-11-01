HOWTO: Install OpenWRT
======================

Summary
-------

This is a quick overview of the very easy process of installing the
OpenWRT open source firmware into a LinkSys WRT54GL. Several much more
detailed sources of documentation exist on this
`1 <http://wiki.openwrt.org/OpenWrtDocs/Installing>`__
`2 <http://wiki.openwrt.org/InstallingWrt54gl>`__, but we include the
bare essentials here for simplicity's sake and also because the bulk of
OpenWRT does not interest us--it only provides us with a working
open-source implementation of an embedded operating system on the
router.

OpenWRT provides a mature environment for exploring your router
hardware, but is not required to run XINU.

Before Starting
---------------

Download OpenWRT Binary
~~~~~~~~~~~~~~~~~~~~~~~

Get the latest OpenWRT binary from
http://downloads.openwrt.org/whiterussian/newest/bin/. The correct file
is ``openwrt-wrt54g-squashfs.bin``

Steps to Install OpenWRT
------------------------

#. Connect to the LinkSys web interface by visiting its address (default
   192.168.1.1) in a web browser.
#. Upload the OpenWRT file as a "firmware update".
#. Watch as your router magically transforms into an OpenWRT-running
   box.

What to do next?
----------------

First it is a good idea to play around with the OpenWRT installation and
get a feel for its web interface. You can also login via ssh to the
router and poke around its directory structure.

The next big step towards a custom operating system on the router is
covered in the next HOWTO in which we will `modify the LinkSys
hardware <HOWTO:Modify the Linksys hardware>`__

Category:HOWTO
