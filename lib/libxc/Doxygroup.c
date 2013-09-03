/**
 * @defgroup libxc C library
 * @brief A simple C library supporting a number of standard functions
 *
 * @section libxc_overview Overview of libxc
 *
 * libxc is a "minimal" C library distributed with Embedded Xinu.  It is intended
 * to be easy to understand rather than high-performance or fully standards
 * compliant.  However, the functions that are implemented are the same as the
 * standard versions, except as documented below.
 *
 * @section libxc_deviations Deviations from standard behavior
 *
 * For various reasons (usually simplicity), libxc deviates from other C
 * libraries in the following ways:
 *
 * - Many standard functions are simply not implemented.
 *
 * - Formatted printing and scanning support only a limited range of format
 *   specifiers.  See _doscan() and _doprnt() for more information.
 *
 * - The ctype functions (e.g. isalpha()) do not handle EOF as specified by the
 *   C standards.
 *
 * - putc() is not implemented in libxc.  In Xinu it's actually a "system call",
 *   and its arguments are reversed compared to the standard putc().  Use
 *   fputc() or putchar() to get standard behavior.
 *
 * - The stdio functions do not buffer the output like a standard implementation
 *   would; instead they write directly to a device descriptor (rather than a
 *   @c FILE stream).
 *
 * - libxc is designed to be used in Embedded Xinu, which does not have a
 *   "userspace" per se.  Therefore, other than printing and scanning (which use
 *   device I/O) and memory allocation, it is primarily intended to provide
 *   functionality that does not rely on any operating system features.
 *
 * - strlcpy() is implemented for convenience, even though this is technically a
 *   BSD extension and not standard.  We do this because several places in
 *   Embedded Xinu were incorrectly calling strncpy() when they expected
 *   behavior equivalent to strlcpy().
 *
 * @section libxc_overrides Platform-specific overrides
 *
 * Sometimes, one would like to build the C library with optimized
 * implementations of certain functions, usually string functions like memcpy()
 * or strlen(), written in assembly language for a particular architecture.  In
 * line with the goals of Embedded Xinu, this is discouraged because this makes
 * it more difficult for people to understand the code and find where a given
 * function is actually defined for a given platform.
 *
 * If you nevertheless still wish to do this, please do not modify the code in
 * libxc itself unless absolutely necessary.  Instead, define a variable @c
 * LIBXC_OVERRIDE_CFILES in the platform-specific platformVars file (e.g. @c
 * compile/platforms/$(PLATFORM)/platformVars) that is a list of C source files
 * in libxc that should not be compiled.  For example, if you override memcpy(),
 * then you would specify
 *
 *     LIBXC_OVERRIDE_CFILES := memcpy.c
 *
 * in platformVars.  You then need to provide your own implementation of the
 * corresponding function(s), but please do it in a platform-specific directory
 * (e.g.  @c system/platforms/$(PLATFORM)) instead of in here.
 *
 * This method still has the limitation that the replacement function(s) will
 * not be included in @c libxc.a itself, only in the kernel as a whole.
 * However, this is inconsequential for Embedded Xinu where everything gets
 * linked into a single kernel image.
 */
