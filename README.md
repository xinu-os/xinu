README: information about the directory tree and how to build an image.

Embedded Xinu, Copyright (C) 2008, 2009, 2010.  All rights reserved.

# Embedded Xinu #

Version: 2.01

 1. What is Embedded Xinu?
 2. Directory Structure
 3. Prerequisites
 4. Installation Instructions
    1. Build Embedded Xinu
    2. Make serial and network connections
    3. Enter Common Firmware Environment prompt
    4. Set IP address
    5. Load image over TFTP
 5. Links


## 1. What is Embedded Xinu? ##

Xinu ("Xinu is not unix", a recursive acronym) is a UNIX-like operating
system originally developed by Douglas Comer for instructional purposes at
Purdue University in the 1980s.

Embedded Xinu is a reimplementation of the original Xinu operating system
on the MIPS processor which is able to run on inexpensive wireless routers
and is suitable for courses and research in the areas of Operating Systems,
Hardware Systems, Embedded Systems, Networking, and Compilers.

## 2. Directory Structure ##

Once you have downloaded and extracted the xinu tarball, you will see a
basic directory structure:

	AUTHORS   device/   lib/     mem/      loader/   README  system/
	compile/  include/  LICENSE  mailbox/  network/  shell/  test/

 * `AUTHORS`  a brief history of contributors to the Xinu operating system
              in it's varying iterations.
 * `compile/` contains the Makefile and other necessities for building the
              Xinu system once you have a cross-compiler.
 * `device/`  contains directories with source for all device drivers in Xinu.
 * `include/` contains all the header files used by Xinu.
 * `lib/`     contains library folders (e.g., `libxc/`) with a Makefile and 
              source for the library
 * `LICENSE`  the license under which this project falls.
 * `loader/`  contains assembly files and is where the bootloader will begin
              execution of O/S code.
 * `mailbox/` contains source for the mailbox message queuing system.
 * `mem/`     contains source for page-based memory protection.
 * `network/` contains code for the TCP/IP networking stack.
 * `README`   this document.
 * `RELEASE`  release notes for the current version.
 * `shell/`   contains the source for all Xinu shell related functions.
 * `system/`  contains the source for all Xinu system functions such as the
              nulluser process (`initialize.c`) as well as code to set up a C
              environment (`startup.S`).
 * `test/`    contains a number of testcases (which can be run using the shell
              command `testsuite`).

## 3. Prerequisites ##

### 3.1 Supported platform with hardware modification ###

To run Embedded Xinu you need a supported router or virtual machine.
Currently, Embedded Xinu supports Linksys WRT54GL, Linksys WRT160NL,
and the Qemu-mipsel virtual machine.  For an updated list
of supported platforms, visit:

http://xinu.mscs.mu.edu/List_of_supported_platforms

In order to communicate with the router, you need to perform a hardware
modification that will expose the serial port that exists on the PCB.  For
more information on this process, see:

http://xinu.mscs.mu.edu/Modify_the_Linksys_hardware

### 3.2 Cross-compiler ###

To build Embedded Xinu you will need a cross-compiler from your host
computer's architecture to MIPSEL (little endian MIPS for the 54GL router)
or MIPS (big endian for the 160NL router).  Instructions on how to do this
can be found here:

http://xinu.mscs.mu.edu/Build_Xinu#Cross-Compiler

### 3.3 Serial communication software ###

Any serial communication software will do. The Xinu Console Tools include
a program called tty-connect which can serve the purpose for a UNIX 
environment.  More information about the Xinu Console Tools can be found 
at:

http://xinu.mscs.mu.edu/Console_Tools#Xinu_Console_Tools

### 3.4 TFTP server software ###

A TFTP server will provide the router with the ability to download and run
the compiled Embedded Xinu image.  

## 4. Installation Instructions ##

### 4.1 Build Embedded Xinu ###

Update the `MIPS_ROOT` and `MIPS_PREFIX` variables in compile/mipsVars to 
correctly point to the cross-compiler on your machine.

Then, from the compile directory, simply run make, which should leave you
with a xinu.boot file.  This is the binary image you need to transfer to
your router for it to run Embedded Xinu.  The default build is for the
WRT54GL router; change the compile/Makefile `PLATFORM` variable for other
builds.  See the compile/platforms directory for supported configurations.

### 4.2 Make serial and network connections ###

After creating the `xinu.boot` image you can connect the router's serial
port to your computer and open up a connection using the following
settings:

 - 8 data bits, no parity bit, and 1 stop bit (8N1)
 - 115200 bps

### 4.3 Enter Common Firmware Environment prompt ###

With the serial connection open, power on the router and immediately start
sending breaks (Control-C) to the device, if your luck holds you will be
greeted with a CFE prompt.

    CFE>

If the router seems to start booting up, you can powercycle and try again.

### 4.4 Set IP address ###

By default, the router will have a static IP address of 192.168.1.1.  If you
need to set a different address for your network, run one of the following
commands:

    ifconfig eth0 -auto                      if you are using a DHCP server 
    ifconfig eth0 -addr=*.*.*.*              for a static IP address

### 4.5 Load image over TFTP ###

On a computer that is network accessible from the router, start your TFTP
server and place the xinu.boot image in the root directory that the server
makes available.

Then, on the router type the command:

    CFE> boot -elf [TFTP server IP]:xinu.boot

If all has gone correctly the router you will be greeted with the Xinu Shell
(`xsh$ `), which means you are now running Embedded Xinu!

## 5. Links ##

### The Embedded Xinu Wiki ###

The home of the Embedded Xinu project

    http://xinu.mscs.mu.edu/

### Dr. Brylow's Embedded Xinu Lab Infrastructure Page ###

More information about the Embedded Xinu Lab at Marquette University

    http://www.mscs.mu.edu/~brylow/xinu/

