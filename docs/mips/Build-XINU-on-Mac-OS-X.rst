Build Xinu on Mac OS X
======================

To setup a MIPS cross-compiler on Mac OS X, we followed almost the exact
same procedure as the tutorial for setting up the cross-compiler in a
Linux environment. The only difference lies in the second configure
command.

In Linux the command looks as follows:

::

    ./configure  \
            --prefix=/usr/local/project/mipsel-dev \
            --target=mipsel \
            --with-sysroot=/usr/local/project/mipsel-dev/mipsel/ \
            --enable-languages=c 
       make
       make install

In Mac OS X you will want to run the same command with different
options, as follows:

::

    ./configure \
        --prefix=/usr/local/project/mipsel-dev \
        --target=mipsel \
        --enable-languages=c \
        --without-headers \
        --disable-intl
       make
       make install

Besides these differences, your cross-compiler setup on Mac OS X should
be the same as the setup in a Linux environment.

.. raw:: html

   <hr/>

*This work is supported in part by NSF grant DUE-CCLI-0737476.*

Category:HOWTO
