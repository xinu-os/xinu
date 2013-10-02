Standard Library
================

The Embedded XINU standard library contains a portion of the functions
defined by the ANSI standard. The functions parallel the ANSI standard C
library as closely as possible.

Input and Output 
-----------------

Formatted output
~~~~~~~~~~~~~~~~

|    **fprintf** - print output to ``dev`` formatted according to the
format string ``fmt``
|        ``int fprintf(int dev, char *fmt, ...)``

|    **printf** - same as ``fprintf (CONSOLE, ...)``
|        ``int printf(...)``

|    **sprintf** - same as ``printf`` except output is written to the
string ``str``
|        ``int sprintf(char *str, char *fmt, ...)``

Formatted input
~~~~~~~~~~~~~~~

*This portion of the library is still unstable.*

Character input and output
~~~~~~~~~~~~~~~~~~~~~~~~~~

|    **fgetc** - read the next character from device ``dev``; return EOF
if end of file or an error occurs
|        ``int fgetc(int dev)``

|    **fputc** - write a character ``c`` to device ``dev``; return EOF
if an error occurs
|        ``int fputc(int c, int dev)``

|    **fgets** - return a newline-terminated string ``s`` of maximum
length ``n`` from device ``dev``; return NULL if end of file or an error
occurs
|        ``char *fgets(char *s, int n, int dev)``

|    **fputs** - write a string ``s`` to device ``dev``; return EOF if
an error occurs
|        ``int fputs(char *s, int dev)``

|    **getchar** - same as ``fgetc(CONSOLE)``
|        ``int getchar()``

|    **putchar** - same as ``fputc(c,CONSOLE``
|        ``int putchar(int c)``

Character Class Tests 
----------------------

Most of the macros defined in the ``ctype.h`` header are used to
identify properties of specific ASCII characters. The macros return
``TRUE`` if a character is a member of a particular category, otherwise
the macro returns ``FALSE``. The following macros are defined to
determine if a character has the listed property:

-  ``isalpha`` - letter
-  ``isupper`` - uppercase letter
-  ``islower`` - lowercase letter
-  ``isdigit`` - digit
-  ``isxdigit`` - hexadecimal digit
-  ``ishexnumber`` - hexadecimal digit
-  ``isspace`` - white space
-  ``ispunct`` - punctuation
-  ``isalnum`` - alphanumeric
-  ``isprshort`` - printable character that is not a letter, digit, or
   white space
-  ``isprint`` - printable character
-  ``iscntrl`` - control character
-  ``isascii``

A few macros in the ``ctype.h`` header convert characters. These macros
convert characters as follows:

-  ``toupper`` - convert letter to uppercase
-  ``tolower`` - convert letter to lowercase
-  ``toascii``

String Functions 
-----------------

The header ``string.h`` consists of functions used to compare and
manipulate strings.

Str functions
~~~~~~~~~~~~~

The main string functions consist of:

|    **String copy** - copies string ``s2`` to string ``s1``; return
``s1``
|        ``char *strcpy(char *s1, const char *s2)``

|    **String concatenate** - concatenate ``s2`` on the end of ``s1``,
``s1``'s space must be large enough; return ``s1``
|        ``char *strcat(char *s1, const char *s2)``

|    **String compare** - compares ``s1`` and ``s2``; return ``s1>s2``:
>0 ``s1==s2``: 0 ``s1<s2``: <0
|        ``int strcmp(const char *s1, const char *s2)``

|    **Character search** - returns a pointer to the location in ``s``
at which which ``c`` appears
|        ``char *strchr (const char *s, int c)``

|    **Reverse character search** - returns a pointer to the location in
s at which which ``c`` last appears
|        ``char *strrchr(const char *s, int c)``

|    **String length** - returns the length of ``s``
|        ``int strlen(const char *s)``

|    **String search** - returns a pointer to the location in ``cs`` at
which ``ct`` appears
|        ``char *strstr (const char *cs, const char *ct)``

Strn functions
~~~~~~~~~~~~~~

Some string functions are also defined with the option to specify length
limitations:

|    **String n copy** - copies string ``s2`` to string ``s1``,
truncating or null padding to always copy ``n`` bytes; return ``s1``
|        ``char *strncpy(char *s1, const char *s2, int n)``

|    **String n concatenate** - concatenate at most ``n`` bytes of
``s2`` on the end of ``s1``, ``s1``'s space must be large enough; return
``s1``
|        ``char *strncat(char *s1, const char *s2, int n)``

|    **String n compare** - compares at most ``n`` bytes of ``s1`` and
``s2``; return ``s1>s2``: >0 ``s1==s2``: 0 ``s1<s2``: <0
|        ``int strncmp(const char *s1, const char *s2, int n)``

Mem functions
~~~~~~~~~~~~~

The string library also includes functions for manipulating objects as
character arrays:

|    **Memory copy** - copy ``n`` bytes of memory from ``cs`` to ``ct``
|        ``void *memcpy(void *s, const void *ct, int n)``

|    **Memory compare** - compare ``n`` bytes of memory at locations
``cs`` and ``ct``
|        ``int memcmp (const void *s1, const void *s2, int n)``

|    **Memory search** - returns a pointer to the location in memory at
which which a ``c`` appears, starting at ``cs`` and searching at most
``n`` bytes
|        ``void *memchr (const void *cs, int c, int n)``

|    **Memory set** - fill ``n`` bytes of memory with ``c`` starting at
``n``
|        ``void *memset (void *s, int c, int n)``

Utility Functions 
------------------

The ``stdlib.h`` header contains a wide array of utility functions for
number conversion, memory allocation, and sorting.

Number conversion
~~~~~~~~~~~~~~~~~

|    **ASCII to integer** - converts and ASCII value ``p`` to an integer
|        ``int atoi(char *p)``

|    **ASCII to long** - converts and ASCII value ``p`` to a long
|        ``long atol(char *p)``

|    **Absolute value** - returns the absolute value of the integer
``arg``
|        ``int abs(int arg)``

|    **Long absolute value** - returns the absolute value of the long
``arg``
|        ``long abs(long arg)``

Memory allocation
~~~~~~~~~~~~~~~~~

|    **Calloc** - returns a pointer to a memory location with space for
``nobj`` objects each of size ``size``
|        ``void *calloc(ulong nobj, ulong size)``

|    **Malloc** - returns a pointer to a memory location with space for
``size`` bytes
|        ``void *malloc(ulong size)``

|    **Free** - deallocates a portion of memory starting at ``p``
|        ``long free(void *p)``

Miscellaneous
~~~~~~~~~~~~~

|    **Zero memory** - zeroes ``len`` bytes of memory starting at ``p``
|        ``void bzero(void *p, int len)``

|    **Random number** - generates a random long
|        ``unsigned long rand(void)``

Diagnostics 
------------

A macro ``ASSERT(int ''expression'')`` is defined in ``kernel.h``. The
``ASSERT`` macro verifies the specified expression is true, otherwise
the function containing the assert will return ``SYSERR``. No
``assert.h`` header file is included in the Embedded XINU standard
library.

Variable Argument Lists 
------------------------

Functions with a variable number of unknown type arguments rely on
functions in the ``stdarg.h`` header to obtain the arguments provided to
the function. A variable of type ``va_list`` must be defined within the
function to hold the variable argument list. The variable holding the
variable argument list must be initialized using the
``va_start(va_list ap, ''lastarg'')`` function, where ``''lastarg''`` is
the name of the argument prior to the variable argument list in the
function signature. Arguments are obtained from the variable argument
list using ``va_arg(va_list ap, ''type'')``, where ``''type''``
specifies the expected type of the next argument in the list. When
argument reading is complete, the function ``va_end(va_list ap)`` is
called, providing the variable argument list as an argument.

Implementation-defined Limits 
------------------------------

The header ``limits.h`` defines maximum and minimum values for the
integral C types. The constants defined are set according to the 32-bit
Mips architecture of the `supported
platforms <List_of_supported_platforms>`__.

Char
~~~~

-  Bits in a character = 8
-  Maximum value of a ``char`` = +127
-  Minimum value of a ``char`` = -128
-  Maximum value of a ``signed char`` = +127
-  Minimum value of a ``signed char`` = -128
-  Maximum value of an ``unsigned char`` (``uchar``) = 255

Int
~~~

-  Maximum value of an ``int`` = +2147483647
-  Minimum value of an ``int`` = -2147483648
-  Maximum value of an ``unsigned int`` = 4294967295

Long
~~~~

-  Maximum value of a ``long`` = +2147483647
-  Minimum value of a ``long`` = -2147483648
-  Maximum value of an ``unsigned long`` (``ulong``) = 4294967295

Short
~~~~~

-  Maximum value of a ``short`` = +32767
-  Minimum value of a ``short`` = -32768
-  Maximum value of an ``unsigned short`` (``ushort``) = 65535

Not Implemented Headers
-----------------------

Some of the ANSI standard library headers are not included in the XINU
standard library. Some headers have been noted as possible later
additions:

-  ``signal.h`` - provides functionality for handling conditions that
   arise during execution including termination and error conditions
-  ``time.h`` - provides functions for date and time formatting and
   determining current date and time; dates and times are not currently
   used in Embedded XINU, but would be more useful after the network
   driver is complete and Embedded XINU is able to synchronize with an
   time server.

The following headers have been excluded due to architectural
limitations and lack of feasibility:

-  ``math.h``
-  ``float.h``
-  ``setjmp.h``
-  ``locale.h``
-  ``errno.h``
-  ``stddef.h``

References
----------

#. Brian Kernighan and Dennis Ritchie. *The C Programming Language*,
   second edition. Prentice Hall.

