Porting Embedded Xinu
=====================

Build system
------------

This section documents how to set up the build system for a new
Embedded Xinu port.

The majority of the build logic is in the Makefile in ``compile/``.
This file is intended to be platform-independent and you should not
need to modify it.  Instead, you should create a new directory in
``compile/platforms/`` containing the files ``platformVars``,
``xinu.conf``, and ``ld.script`` for your platform.

platformVars
~~~~~~~~~~~~

``platformVars`` is in Makefile syntax and must contain the following
definitions:

- A Makefile rule to generate the file ``$(BOOTIMAGE)``, which is by default
  ``xinu.boot``, from ``xinu.elf``.  ``xinu.elf`` will be the kernel in ELF format,
  while ``xinu.boot`` is normally expected to be the actual bootable kernel that
  is the final result of the build.  This rule may simply copy the file, or it
  may use **objcopy** to change the binary format.  (Use ``$(OBJCOPY)`` to get the
  correct **objcopy** for the target platform.)  If your platform does things very
  differently you can override ``$(BOOTIMAGE)`` to something else.

- ``APPCOMPS`` must be set to the list of Xinu application components
  to build in.  Each item in the list corresponds to a toplevel
  directory, such as ``network``.  Note that certain toplevel
  directories, such as ``lib/`` and ``system/``, are not considered
  "application" components and should not be listed here.

- ``DEVICES`` must be set to the list of Xinu device drivers to build
  in.  Each item in the list corresponds to the name of a directory in
  ``device/``.  Device drivers added here in many cases also require
  configuration in ``xinu.conf``; see the section below.

- ``BUGFLAG`` must be set to an appropriate compiler flag to enable
  debugging information.

- ``ARCH_ROOT`` must be set to the default directory containing the
  compiler binary (ending with a slash).  The exact value is not
  important because it can be overridden on build.

- ``ARCH_PREFIX`` must be set to the default compiler target prefix.

- ``OCFLAGS`` must be set to the appropriate flags to pass to
  **objcopy** to turn a raw binary file into an object file that can
  be linked into the kernel.

``platformVars`` may modify the following definitions:

- ``FLAGS`` can be modified to add additional compiler flags.

- ``ASFLAGS`` can be modified to add additional assembler flags.
  These will be passed directly to the assembler, so do *not* prefix
  these with ``-Wa,``.  For the GNU assembler see ``man as`` or ``info
  as`` for available flags.

- ``LDFLAGS`` can be modified to add additional linker flags.  These
  will be passed directly to the linker, so do not prefix these with
  ``-Wl,``.  For the GNU linker see ``man ld'` or ``info ld`` for
  available flags.

- ``LDLIBS`` can be modified to add needed external libraries.
  Besides possibly adding libgcc (``-lgcc``) if your architecture has
  certain quirks like the need for division to be emulated in
  software, you probably don't need to add anything here.

- ``INCLUDE`` can be modified to add additional include directories.  Prefix each
  with ``-I``.  The toplevel Makefile handles adding these to
  ``CFLAGS`` and ``ASFLAGS`` as appropriate.  You probably don't need
  to add anything here, especially because
  ``system/platforms/$(PLATFORM)`` is already added by default.

- ``DEFS`` can be modified to add additional defines.  Prefix each
  with ``-D``.  The toplevel Makefile handles adding these to
  ``CFLAGS`` and ``ASFLAGS`` as appropriate.  If you need to add
  conditional C code specifically for your platform in existing source
  code (please avoid it whenever possible...) you should define a
  constant like ``_XINU_PLATFORM_???_``, where ``???`` is your
  platform.

Still furthermore, you can optionally set the following variables:

- ``PLATCLEAN`` to be the name of a platform-specific target that will
  be executed when ``make realclean`` is run.

- ``LIBXC_OVERRIDE_CFILES``
  (see :ref:`libxc_overrides` for explanation).

- ``PLATFORM_NAME`` to change the release provided at the top of the
  generated documentation.

xinu.conf
~~~~~~~~~

``xinu.conf`` is the file used for configuring Xinu's static device
table.  See the existing examples in the ``compile/platforms/``
directory for the format.  This file is used to generate
``system/conf.c`` and ``system/conf.h``.  You should keep the devices
defined in ``xinu.conf`` in sync with the components actually compiled
into the kernel via the ``DEVICES`` variable in ``platformVars``.

ld.script
~~~~~~~~~

``ld.script`` is the linker script used to link the kernel.  This is
used to customize the layout of the resulting kernel image, including
the address at which it is compiled to run at.  See the existing
examples in ``compile/platforms/``.

Architectures
~~~~~~~~~~~~~

Multiple Xinu platforms may have the same underlying "architecture",
such as ARM or MIPS.  If the architecture of your platform is already
listed in ``compile/arch/``, then there are two shortcuts you may be
able to take:

- ``platformVars`` can be shortened by including the corresponding ``platformVars``
  in ``compile/arch/``, which will handle many of the definitions for you.
- If you do not include a linker script (``ld.script``) in
  ``compile/platforms/``, then the linker script in ``compile/arch/``
  will be used, if present.  (This requires including the
  corresponding architecture ``platformVars``.)  This script may be
  sufficient for your platform.

The exact contents of the ``platformVars`` in ``compile/arch/`` is
dependent on the architecture; see the existing examples.
