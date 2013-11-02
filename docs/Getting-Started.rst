Getting started with Embedded Xinu
==================================

This section describes how to download and compile :doc:`Embedded Xinu
</Introduction>`, assuming you are using a UNIX operating system such
as Linux or Mac OS X, or at least a UNIX-compatible environment such
as `Cygwin <http://www.cygwin.com>`__.

.. contents::
   :local:

.. _downloading:

Downloading the source code
---------------------------

Stable versions of Embedded Xinu may be downloaded from
http://xinu-os.org/Downloads.

The development version (recommended, as of this writing) is stored in
a repository using the `git source code management system
<http://git-scm.com/>`__.  To download it, `install git
<http://git-scm.com/book/en/Getting-Started-Installing-Git>`__ and run
the following command:

.. code-block:: none

    $ git clone https://github.com/xinu-os/xinu

You then should have a copy of the source:

.. code-block:: none

    $ cd xinu
    $ ls -F
    apps/    compile/  docs/     lib/     loader/   mem/ README.md  system/
    AUTHORS  device/   include/  LICENSE  mailbox/  network/  shell/ test/

Note that XinuPi is licensed under a BSD-style license; see the
copyright information in the source distribution for more details.

Choosing a platform
-------------------

See the :ref:`list of platforms supported by Embedded Xinu
<supported_platforms>`.

.. note::
   Each supported platform corresponds to a subdirectory of
   ``compile/platforms/``.

If you do not have "real embedded hardware" available and simply would
like to try out Embedded Xinu from your laptop or desktop, you can use
the :doc:`mipsel-qemu <mips/Mipsel-qemu>` platform, which allows you
to run a little-endian MIPS build of Embedded Xinu from within the
`QEMU processor emulator <http://qemu.org>`__.

.. _cross_compiler:

Setting up a cross-compiler
---------------------------

Since most of Embedded Xinu's supported platforms do *not* share the
same processor architecture as an x86 workstation, building Embedded
Xinu typically requires an appropriate :wikipedia:`cross compiler` for
the C programming language.

Currently, only the `gcc compiler <http://gcc.gnu.org>`__ is
supported.  We have not yet tested clang as an alternative to gcc,
although there is little reason why it should not work.

Option 1: Install cross-compiler from repository
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some Linux distributions already have popular cross compilers
available in their software repositories.  When available, this can be
used as a quick alternative to building from source.

Option 2: Build cross-compiler from source
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section documents how to build and install binutils and gcc from
source in a cross-compiler configuration.

Native development environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before you can build anything from source, you first need appropriate
development tools for your native platform, such as **gcc** and
**make**.

- On Linux systems, these tools can be found in the software
  repositories under various names and groupings, depending on the
  Linux distribution.
- On Windows via `Cygwin <http://www.cygwin.com>`__, these tools can
  be found under the "devel" category when you run the setup program.
- On Mac OS X, these tools come with `Xcode
  <https://developer.apple.com/xcode/>`__.

binutils
^^^^^^^^

Before building the C compiler itself, the corresponding binary
utilities including the :wikipedia:`assembler <Assembler (computing)>`
and :wikipedia:`linker <Linker (computing)>` must be installed.

.. note::

   Good practice when building any software package is to use a normal
   user account, and only acquire root privileges with ``sudo`` for
   installation (step 6 below).

1. Download a recent release of `GNU binutils
   <https://www.gnu.org/software/binutils/>`__, for example:

   .. code-block:: none

      $ wget ftp://ftp.gnu.org/gnu/binutils/binutils-2.23.tar.gz

2. Untar the binutils source:

   .. code-block:: none

      $ tar xvf binutils-2.23.tar.gz

3. Create and enter a build directory:

   .. code-block:: none

      $ mkdir binutils-2.23-build
      $ cd binutils-2.23-build

4. Configure binutils for the appropriate target, for example:

   .. code-block:: none

      $ ../binutils-2.23/configure --prefix=/opt/mipsel-dev --target=mipsel \
               --disable-nls

   The argument given to ``--prefix`` is the location into which to
   install the binutils, and is of your choosing.  Typical locations
   would be a subdirectory of ``/opt`` or ``/usr/local``.  (Note that
   installing into these locations requires ``sudo`` privilege in
   step 6.  Normally, it is also possible to install software into a
   user's home directory, which does not require the ``sudo``
   privilege.)

   The argument given to ``--target`` is the target which the binutils
   will target, and must be set appropriately for the desired Embedded
   Xinu platform.  A few of the available targets are:

   - ``mipsel`` for little-endian MIPS platforms, such as the
     :doc:`mips/WRT54GL` and :doc:`mips/Mipsel-qemu`.
   - ``mips`` for big-endian MIPS platforms.
   - ``arm-none-eabi`` for ARM platforms, such as the
     :doc:`arm/rpi/Raspberry-Pi`.

   ``--disable-nls`` simply saves time and space by not supporting any
   human languages other than English.  You can skip this option if
   you want.

5. Build binutils:

   .. code-block:: none

      $ make

6. Install binutils:

   .. code-block:: none

      $ sudo make install

gcc
^^^

1. Download a recent release of the `GNU Compiler Collection
   <https://gcc.gnu.org>`__, for example:

   .. code-block:: none

      $ wget ftp://ftp.gnu.org/gnu/gcc/gcc-4.8.2/gcc-4.8.2.tar.bz2

2. Untar the gcc source:

   .. code-block:: none

      $ tar xvf gcc-4.8.2.tar.bz2

3. Create and enter a build directory:

   .. code-block:: none

      $ mkdir gcc-4.8.2-build
      $ cd gcc-4.8.2-build

4. Configure gcc for the appropriate target, for example:

   .. code-block:: none

      $ ../gcc-4.8.2/configure --prefix=/opt/mipsel-dev --target=mipsel \
                --enable-languages=c,c++ --without-headers --disable-nls

   ``--prefix`` and ``--target`` must be exactly the same as those
   chosen for the binutils installation.

   ``--enable-languages=c,c++`` ensures that only C and C++ compilers
   are built, not the compilers for other languages such as Ada and
   Fortran that are also supported by the GNU Compiler Collection.
   Note: Embedded Xinu does not actually contain C++ code, so if
   desired this could be stripped down to simply
   ``--enable-languages=c``.

   ``--without-headers`` is needed when there is no libc (standard C
   library) installed for the target platform, as is the case here.

   ``--disable-nls`` simply saves time and space by not supporting any
   human languages other than English.  You can skip this option if
   you want.

5. Build gcc:

   .. code-block:: none

      $ make all-gcc all-target-libgcc

   .. tip::

      gcc can take a while to build (upwards of half an hour).  You
      can add the argument ``-jN`` to **make**, where N is an integer,
      to run multiple compilation jobs in parallel.

6. Install gcc:

   .. code-block:: none

      $ sudo make install-gcc install-target-libgcc

Testing the cross compiler
^^^^^^^^^^^^^^^^^^^^^^^^^^

First, for convenience you may wish to make the cross-utilities
available under their unqualified names by updating ``$PATH``, for
example:

.. code-block:: none

   export PATH="$PATH:/opt/mipsel-dev/bin"

The above should go in a shell startup file such as ``$HOME/.bashrc``.

Test the compiler by creating a file ``test.c``:

.. code-block:: c

    void f(void)
    {
    }

and compiling it with, for example::

    mipsel-gcc -c test.c

This should succeed and produce a file ``test.o`` without any error
messages.

.. _compiling:

Compiling Embedded Xinu
-----------------------

Having built a cross-compiler if needed, compiling Embedded Xinu now
requires running **make** to process the ``Makefile`` in the
``compile/`` directory and specifying an appropriate ``PLATFORM``, for
example:

.. code-block:: none

   $ make -C compile PLATFORM=wrt54gl

Additional details follow.

.. _makefile_variables:

Makefile variables
~~~~~~~~~~~~~~~~~~

Several variables can be defined on the **make** command line to
customize the build.

* ``PLATFORM`` specifies the name of a directory in
  ``compile/platforms/`` that is the Embedded Xinu platform for which
  to build the kernel.

* ``COMPILER_ROOT`` specifies the location of the executables for the compiler and
  binutils necessary to compile, assemble, and link code for the target
  platform.  ``COMPILER_ROOT`` must include any target prefix that the executables
  may be prefixed with.  Example for ARM-based platforms:
  ``/opt/arm-dev/bin/arm-none-eabi-``.  Or, if the executables are on
  your ``$PATH``, you could simply specify, for example,
  ``arm-none-eabi-``; however, that (or the corresponding prefix for a
  non-ARM-based ``PLATFORM``) is already the default.

* ``DETAIL`` can be defined as ``-DDETAIL`` to enable certain
  debugging messages in Embedded Xinu.

* ``VERBOSE`` can be defined to any value to cause the build system to
  print the actual command lines executed when compiling, linking,
  assembling, etc.

To override any of the above variables, you must pass it as an argument to
**make**, like in the following example::

    $ make PLATFORM=arm-rpi

.. _makefile_targets:

Makefile targets
~~~~~~~~~~~~~~~~

The following Makefile targets are available:

* **xinu.boot**
    Compile Embedded Xinu normally.  This is the default target.

* **debug**
    Same as xinu.boot, but include debugging information.

* **docs**
    Generate the Doxygen documentation for Embedded Xinu.  This requires that
    Doxygen is installed.  Note: to eliminate irrelevant details in the
    documentation, the documentation is parameterized by platform; therefore,
    the exact documentation that's generated will depend on the current setting
    of ``PLATFORM`` (see :ref:`makefile_variables`).

* **clean**
    Remove all object files.

* **docsclean**
    Remove documentation generated by ``make docs``.

* **realclean**
    Remove all generated files of any kind.

The above covers the important targets, but see the
``compile/Makefile`` for a few additional targets that are available.

.. note::
    Older versions of Embedded Xinu had a ``make depend`` target to
    generate header dependency information.  This has been removed because
    this information is now generated automatically.  That is, if you
    modify a header, the appropriate source files will now be recompiled
    automatically.

Next steps
----------

Typically, after :ref:`compiling Embedded Xinu <compiling>`, a file
``xinu.boot`` containing the kernel binary is produced.  Actually
running this file is largely platform-dependent.  Just a few examples
are:

- Raspberry Pi:  See :ref:`raspberry_pi_booting` and
  :ref:`xinupi_getting_started`.
- Mipsel-Qemu:  See :doc:`/mips/Mipsel-qemu`.

Places to go next:

- :doc:`features/index`
- :doc:`teaching/index`

Other resources
---------------
- `GCC Cross-Compiler (OSDev Wiki) <http://wiki.osdev.org/GCC_Cross-Compiler>`__
