Standard C Library
==================

.. contents::
   :local:

Overview
--------

The XINU C library, or **libxc**, is a "minimal" standard C library
distributed with XINU.  It is intended to be easy to understand rather
than high-performance or fully standards compliant.  However, the
functions that are implemented are mostly the same as the standard
versions except as documented below.

API
---

For the sake of reducing redundancy, the full API (functions and
macros) provided by libxc is not documented on this page.  Instead,
every function implemented in the :source:`lib/libxc` directory has a
detailed comment describing its behavior.  Note that every C source
file in this directory implements a separate externally visible
function.

The library headers are:

=======================================   ==========================
Header                                    Description
=======================================   ==========================
:source:`ctype.h <include/ctype.h>`       Character types
:source:`limits.h <include/limits.h>`     Limits of integer types
:source:`stdarg.h <include/stdarg.h>`     Variable argument lists
:source:`stdint.h <include/stdint.h>`     Fixed-width integer types
:source:`stdio.h <include/stdio.h>`       Standard input and output
:source:`stdlib.h <include/stdlib.h>`     Standard library definitions
:source:`string.h <include/string.h>`     String operators
=======================================   ==========================

:source:`stddef.h <include/stddef.h>` is also present and defines some
XINU-specific types in addition to the standard ``offsetof``,
``size_t``, and ``NULL``.

Deviations from standard behavior
---------------------------------

For various reasons (usually simplicity), libxc deviates from other C
libraries in the following ways:

- Many standard functions (and some headers) are simply not
  implemented.  Examples:  Floating-point mathematics functions;
  ``setjmp()`` and ``longjmp()``; wide character support; locale
  support; time functions; complex arithmetic.

- Formatted printing and scanning support only a limited range of
  format specifiers.  See :source:`_doscan() <lib/libxc/_doscan.c>`
  and :source:`_doprnt() <lib/libxc/_doprint.c>` for more information.

- The ctype functions declared in :source:`include/ctype.h` do not
  handle ``EOF`` (end-of-file) as specified by C99.

- ``putc()`` is not implemented in libxc.  In Xinu it's actually a
  "system call", and its arguments are reversed compared to the
  standard ``putc()``.  Use ``fputc()`` or ``putchar()`` to get
  standard behavior.

- The stdio functions do not buffer the output like a standard
  implementation would; instead they write directly to a device
  descriptor (rather than a ``FILE`` stream).

- :source:`strlcpy() <lib/libxc/strlcpy.c>` is implemented, even
  though this is technically a nonstandard BSD extension.  We do this
  because several places in XINU were incorrectly calling
  :source:`strncpy() <lib/libxc/strncpy.c>`
  when they expected behavior equivalent to
  :source:`strlcpy() <lib/libxc/strlcpy.c>`.

.. _ Standard C Library#Platform-specific overrides:

Platform-specific overrides
---------------------------

Sometimes, one would like to build the C library with optimized
implementations of certain functions, usually string functions like
:source:`memcpy() <lib/libxc/memcpy.c>`
or
:source:`strlen() <lib/libxc/strlen.c>`
written in assembly language for a particular architecture.  In line
with the goals of XINU, this is discouraged because this makes it more
difficult for people to understand the code and find where a given
function is actually defined for a given platform.

If you nevertheless still wish to do this, please do not modify the
code in libxc itself unless absolutely necessary.  Instead, define a
variable ``LIBXC_OVERRIDE_CFILES`` in the platform-specific
platformVars file (e.g.
``compile/platforms/$(PLATFORM)/platformVars``) that is a list of C
source files in libxc that should not be compiled.  For example, if
you override ``memcpy()``, then you would specify::

    LIBXC_OVERRIDE_CFILES := memcpy.c

in platformVars.  You then need to provide your own implementation of
the corresponding function(s), but please do it in a platform-specific
directory (e.g.  ``system/platforms/$(PLATFORM)``) instead of in here.

This method still has the limitation that the replacement function(s)
will not be included in ``libxc.a`` itself, only in the kernel as a
whole.  However, this is inconsequential for XINU where everything
gets linked into a single kernel image.

References
----------

- :wikipedia:`C standard library - Wikipedia <C standard library>`
- `C99 standard <http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf>`_
-  Brian Kernighan and Dennis Ritchie. *The C Programming Language*,
   second edition. Prentice Hall.
