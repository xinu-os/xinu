Curses
======

.. raw:: mediawiki

   {{Historical}}

:wikipedia`Ncurses` is a programming library API for implementing text
user interfaces. Currently, XINU's interface for output is limited to
sequential lines of output using the :doc:`TTY driver
</features/TTY-Driver>` and :doc:`serial driver </mips/UART-Driver>`.
The addition of an ncurses library in XINU would allow for more
advanced text based interfaces.

Why it is called curses
-----------------------

Ncurses is designed to be a terminal independent API for text based
interfaces. This terminal independence requires a multi-layered internal
implementation of the ncurses library. Adding the actual ncurses
implementation into XINU would double the size of XINU boot image. Also,
the standard ncurses implementation is highly complex, a contrast to
XINU's clean code base. XINU only has one terminal so the additional
complexity of terminal independence is unnecessary for XINU. Therefore,
attempts are being made to implement a clean ncurses library for XINU.

Starting from the original
`curses <http://en.wikipedia.org/wiki/Curses_(programming_library)>`__
library is fairly straight forward. However, original curses does not
provide color, text formatting, or other "cool" features that would make
a text based interface for XINU worthwhile. Instead, the ncurses API,
which includes these additional features, must be implemented for XINU.
The ncurses API is much more complex. For example, multiple functions
add characters to the screen: ``addch``, ``waddch``, and ``mvaddch``,
``mvwaddch``. The main function is ``waddch``, while the other three are
macroed to move the cursor or specifing a specific window to which the
character should be added.

Major aspects
-------------

Refresh
~~~~~~~

Refreshing the terminal to display the desired output is the most
crucial aspect of the curses library.

Display output
~~~~~~~~~~~~~~

Format output
~~~~~~~~~~~~~

The curses library can change colors and text decoration (bold,
underline, etc.) to provide output other than plain text.

Receive input
~~~~~~~~~~~~~

Resources
---------

-  Strang, John. *Programming with curses*. Sebastopol, CA: O'Reilly,
   1986.
-  Gookin, Dan. *Programmer's Guide to nCurses*. Indianapolis: Wiley,
   2007.
-  `Announcing ncurses 5.6 <http://www.gnu.org/software/ncurses/>`__
