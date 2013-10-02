Build Xinu on Windows
=====================

Preparation
-----------

First, make sure that you have all packages in the "*Devel*\ " category
of `Cygwin <http://www.cygwin.com>`__ installed on your Windows machine.
This will provide a UNIX-like background for performing gcc compilation
and allows the use of other UNIX commands (*ie*. `` cat``,
`` hostname``, *etc*).

Next, you will need to create two directories:
`` /usr/src/build-binutils `` and `` /usr/src/build-gcc ``.

Then, you will want to download the appropriate binary utilities. We
downloaded `binutils <http://www.gnu.org/software/binutils/>`__ version
`2.17 <http://ftp.gnu.org/gnu/binutils/binutils-2.17.tar.gz>`__ and
saved it in the directory `` /usr/src/build-binutils``.

Then, download the GNU C Compiler. We downloaded
`GCC <http://ftp.gnu.org/gnu/gcc/gcc-4.1.1/>`__ version 4.1.1 and saved
it in the directory `` /usr/src/build-gcc``.

Once these are both downloaded, navigate to their respective directories
and untar the tar files.

Build Process
-------------

BINUTILS
~~~~~~~~

First, navigate to the directory of the newly untarred files. For us the
directory was `` /usr/src/build-binutils/binutils-2.17``. Once there,
run the following commands:

::

    ./configure --target=mipsel --prefix=/usr/cross --disable-nls
    make all install

You will want to add these files to the path with the command:

::

    export PATH=$PATH:/usr/cross/bin

**NOTE:** you can also add the command to your `` ~/.bashrc `` file to
automatically add that directory to `` PATH `` everytime Cygwin is
started.

GCC
~~~

Again, navigate to the directory of the newly untarred files. For us the
directory was `` /usr/src/build-gcc/gcc-4.1.1``. Once there, run the
following commands:

::

    ./configure --target=mipsel --prefix=/usr/cross --disable-nls -enable-languages=c --without-headers
    make all-gcc install-gcc

Running on Windows
------------------

The previous setup should be sufficient for compiling and running XINU
inside of a Cygwin bash shell. However, if you want to run it on regular
Windows you will need to prepend the Cygwin paths to the Windows
environment because in Windows XP *hostname* acts differently than
*hostname* under UNIX. We solved this problem by adding
`` C:\cygwin\bin `` and `` C:\cygwin\usr\cross\bin `` to the Windows XP
environment PATH first, before any other directories. This way,
*hostname* from the Cygwin installation will be the first occurrence of
"hostname" when the XINU code tries to execute the command.

Alternatively, if you are setting up one or more frontend clients that
use a Windows operating system you could use the Windows tool set to
connect to the target pool or resource. This tool set was initially
released in the Spring of 2008 and is entitled WinXINU.

.. raw:: html

   <hr/>

*This work is supported in part by NSF grant DUE-CCLI-0737476.*

Category:HOWTO
