Build Xinu
==========

The new MIPS port of Embedded XINU has been released. See the Downloads
tab for more information. A revision of Doug Comer's venerable
*Operating System Design - The XINU Approach* textbook is in progress.

Cross-Compiler
--------------

In order to compile Embedded MIPS kernels on a workstation that is not
itself a MIPS processor, it is necessary to build and install an
appropriate `cross compiler <wikipedia:Cross Compiler>`__. There are
many ways to accomplish this; for reference, we list the specific
versions and steps we used for our installation.

**NOTE:** the following tutorial describes the steps for creating a MIPS
cross-compiler in a Linux environment. See our other wiki pages for
tutorials on building cross-compilers for `Windows
XP <HOWTO:Build XINU on Windows XP>`__ or `Mac OS
X <HOWTO:Build XINU on Mac OS X>`__.

As always, one should be wary of installing unfamilar software as the
root user of the system. All of the steps below have been carried out as
a lesser-privileged user with write access to the necessary directories.

Build binutils
~~~~~~~~~~~~~~

First, we downloaded, compiled, and installed the appropriate binary
utilities. We downloaded
`binutils <http://www.gnu.org/software/binutils/>`__ version
`2.21 <http://ftp.gnu.org/gnu/binutils/binutils-2.21.tar.gz>`__,
untarred the source code, and ran the following commands inside the new
directory created by untarring the download:

| ``  ./configure  --prefix=/usr/local/project/mipsel-dev --target=mipsel``
| ``  make``
| ``  make install``

Note about cross-compiler location
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have chosen the path "/usr/local/project/mipsel-dev" to host our
cross-compiler installation. Whatever path is used here must be
reflected in the XINU build configuration file,
"compile/platforms/\*/platformVars" when you arrive at that step.

Link include directory
~~~~~~~~~~~~~~~~~~~~~~

We are not building a true, full-blown UNIX cross-compiler here, and do
not need a proper installation of the platform-specific C libraries;
XINU has its own small libraries that compile with the kernel. However,
the GCC compilation will want to see appropriate library headers, so we
cheat here by linking the platform-specific include directory to the
host machine's include directory.

| ``  mkdir -p /usr/local/project/mipsel-dev/mipsel/usr``
| ``  ln -s /usr/include /usr/local/project/mipsel-dev/mipsel/usr/include``

Build GNU C Compiler
~~~~~~~~~~~~~~~~~~~~

Second, we downloaded, patched, compiled and installed the GNU C
Compiler. We downloaded `GCC <http://gcc.gnu.org/>`__ version 4.6.1. We
added the newly compiled binutils into the shell path
(/usr/local/project/mipsel-dev/bin) for the gcc compilation to find
them. This is also known to work with `GCC <http://gcc.gnu.org/>`__
version 4.1.2 and 4.2.0 unpatched, using `this
script <http://www.mscs.mu.edu/~brylow/xinu/fakelibdetection.sh>`__ to
get around installing a bunch of platform-specific UNIX libraries.

| ``  ./configure  --prefix=/usr/local/project/mipsel-dev --target=mipsel --with-sysroot=/usr/local/project/mipsel-dev/mipsel/ --enable-languages=c``
| ``  make``
| ``  make install``

Rejoice
~~~~~~~

If all has gone well, you should now have a gcc cross-compiler from your
host's native architecture to little-endian MIPS:

``  /usr/local/project/mipsel-dev/bin/mipsel-gcc``

Remember the path to this file because later you'll need to double check
some building variables to make sure they point to the correct location
of your cross-compiler.

Building the XINU Image
-----------------------

Once you have downloaded and extracted the xinu tarball, you will see a
basic directory structure:

::

    AUTHORS  device   lib      loader   README  system
    compile  include  LICENSE  mailbox  shell   test

``AUTHORS`` is a brief history of contributors to the XINU operating
system in it's varying iterations.

``compile/`` contains the Makefile and other necessities for `building
the XINU system <Build System>`__ once you have a cross-compiler.

``device/`` contains the source for all device drivers, including the
tty and uart driver.

``include/`` contains all the header files used by XINU.

``lib/`` contains a folder (libxc/) with a Makefile and source for the
library, as well as a binary blob which contains the pre-compiled
library.

``LICENSE`` is the license under which this project falls.

``loader/`` contains assembly files and is where the bootloader will
begin execution of O/S code.

``mailbox/`` contains the source for the mailbox message-passing
implementation.

``README`` is this document.

``shell/`` contains the source for all shell related functions.

``system/`` contains the source for all system functions such as the
nulluser process (initialize.c) as well as code to set up a C
environment (startup.S).

``test/`` contains a number of testcases (which can be run using the
shell command testsuite).

--------------

First, it is a good idea to read up on `building the XINU
system <Build System>`__. Next, you'll want to check your `` mipsVars ``
file in the `` compile `` directory. The file should look something like
this:

::

    MIPS_ROOT = /usr/local/project/mipsel-dev/bin
    MIPS_PREFIX = mipsel-

    COMPILER_ROOT = ${MIPS_ROOT}/${MIPS_PREFIX}

    CC       = ${COMPILER_ROOT}gcc
    CPP      = ${COMPILER_ROOT}cpp
    LD       = ${COMPILER_ROOT}ld
    AS       = ${COMPILER_ROOT}as
    AR       = ${COMPILER_ROOT}ar
    MAKEDEP  = `which makedepend`

    DOCGEN   = doxygen

    # DETAIL   = -DDETAIL

    DEFS     =
    INCLUDE  = -I../include

    #flag for producing GDB debug information
    BUGFLAG = -ggdb

    CFLAGS = -O0 -Wall -Werror -Wstrict-prototypes -Wno-trigraphs            \
                 -nostdinc -fno-builtin -fno-strict-aliasing -fno-common \
             -fomit-frame-pointer -fno-pic -ffunction-sections -G 0  \
             -mlong-calls -mno-abicalls -mabi=32 -march=mips32       \
             -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap        \
             ${DEBUG} ${INCLUDE} ${DETAIL} ${DEFS} -c


    ASFLAGS  = ${INCLUDE} ${DEBUG} -march=mips32 -mips32

The important thing to note on this file is the `` MIPS_ROOT ``
variable. It needs to point to the directory containing the
cross-compiler. If you followed the commands in the tutorial exactly,
then the value of the `` MIPS_ROOT `` variable in the code above should
be the correct value.

Now you'll want to actually create your boot image. In the
`` compile/ `` directory execute the following commands: ``make clean ``
and `` make``. The `` make `` command will let you know if you have any
compiling errors or warnings. If there are none, then you should have
successfully created a XINU boot image located in the file
`` xinu.boot `` in the `` compile/ `` directory. Remember that each time
you make changes to source files, to recompile and create a new XINU
boot image you have to execute `` make clean `` and then `` make``.

What to do next?
----------------

Now that you have successfully built a XINU boot image you're ready to
use that file to actually `boot XINU <HOWTO:Deploy_Xinu>`__ on your
backend router.

.. raw:: html

   <hr/>

*This work is supported in part by NSF grant DUE-CCLI-0737476.*

Category:HOWTO
