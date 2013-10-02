Shell
=====

The XINU shell is a `subsystem <Modules>`__ designed as an interface for
interacting with the operating system.

How it works
------------

The shell relies on the `TTY driver <TTY_Driver>`__ to receive user
input and provide output to the user. When XINU starts, a shell process
is spawned for each `serial port <UART_Driver>`__ (or TTY). When a user
enters a command the
```lexan`` <http://xinu.mscs.mu.edu/docs/lexan_8c.html>`__ function
divides the string of input into tokens. Command name, arguments, quoted
strings, backgrounding, and redirection tokens are all recognized and
divided by ``lexan``.

After the command is parsed, the shell uses the tokens to properly
execute the given command. The shell first checks for the backgrounding
ampersand ('&'), which should only appear as the last token. The shell
is designed to handle redirection, but does not currently do so since
XINU's file system is in development.

Next, the command is looked up in the command table defined at the top
of ``[http://xinu.mscs.mu.edu/docs/shell_8c.html shell/shell.c]``. Each
entry in the command table follows the format of command name, is the
command built-in (ie can the command run in the background), and the
function that executes the command:
``{"command_name", TRUE / FALSE, xsh_function},``.

Built-in commands are executed by calling the function that implements
the command. All other commands are executed by creating a new process.
If the user did not include the backgrounding flag in the input, the
shell waits until the command process has completed before asking for
more input.

Running commands
----------------

The current distribution of the XINU shell is equipped with some basic
commands. The majority of the commands provide information about the
current state of the system including its processes, memory, and
hardware. A full list of commands can be obtained from the shell by
using the ``help`` command. Help on a specific command can be obtained
using ``COMMAND --help`` or ``help COMMAND``.

-  ``clear``: clears the shell
-  ``exit``: quits the XINU shell
-  ``gpiostat``: displays the current status of the GPIO pins
-  ``help``: displays a list of commands in the XINU shell
-  ``kill PID``: kills a process number ``PID``
-  ``led DESCRIPTOR STATUS``: turns an led on or off
-  ``memstat``: displays the current memory usage and prints the free
   list
-  ``memdump``: dumps a region of memory
-  ``ps``: displays a table of running processes
-  ``reset``: soft powercycles the backend
-  ``sleep DELAY``: sleep for ``DELAY`` seconds
-  ``test``: can be used for building test programs, all builds should
   simply return ``OK``
-  ``testsuite``: run a series of tests to see if the system is
   functioning properly
-  ``uartstat UARTNUM``: displays statistics for `uart <UART>`__
   ``UARTNUM``

Adding commands
---------------

The shell is designed to be expandable, allowing users to add their own
commands. The code that runs the shell (``shell/shell.c``) and the
command parser (``shell/lexan.c``) do not need to change when a new
command is added. The majority of the work goes into writing the actual
command. After the command is written, three items must be added to the
system:

-  the function prototype needs to be added to the header file
   (``include/shell.h``),
-  the command table (``shell/shell.c``) must be updated, and
-  the make file (``shell/Makerules``) must build the file containing
   the function.

Writing the function
~~~~~~~~~~~~~~~~~~~~

The command should be given its own C source file in the ``shell``
directory, following the naming convention ``xsh_command.c``. All
command files should include ``kernel.h`` and ``shell.h``, along with
any other headers necessary for the command. Function names for commands
follow the same naming convention as the source file: ``xsh_command``.
The method signature for a command is: command xsh\_command(ushort
stdin, ushort stdout, ushort stderr, ushort nargs, char \*args[])

Within the command, arguments are accessed via the ``args`` array. The
command name is located in ``arg[0]``. Subsequent arguments, up to
``nargs`` are accessed via ``arg[n]``. Error checking of arguments is
the responsibility of the command function. It is good practice to check
for the correct number of arguments; remember the command name is
counted in ``nargs``, so a command without any arguments should have
``nargs == 1``. Although not required, command functions should also
allow for an argument of ``--help`` as ``arg[1]``. This argument should
cause the command to print out usage information. When a user types
``help COMMAND`` in the shell, the ``COMMAND`` is called with the
``--help`` argument.

Additional code within the command function depends on what the command
does. After the command is completed it should return ``OK``.

Add to command table
~~~~~~~~~~~~~~~~~~~~

After the command function is written, the command needs to be added to
the command table so the shell is aware of the command. The command
table is an array of ``centry`` (command entry) structures defined in
``shell/shell.c``. Each entry in the command table follows the format of
command name, is the command built-in (ie can the command run in the
background), and the function that executes the command:
``{"command_name", TRUE / FALSE, xsh_function},``.

Add to header and makefile
~~~~~~~~~~~~~~~~~~~~~~~~~~

To complete the process, add the function prototype to the shell header
file ``include/shell.h``:

.. code:: c

    command xsh_command(ushort, char *[]);

Lastly, add the command function source file to the makefile
(``shell/Makerules``) under the ``C_FILES`` group to ensure the command
is compiled into the XINU boot image.

Example
~~~~~~~

We will run through a brief implementation of adding an echo command to
the system.

Write the function
^^^^^^^^^^^^^^^^^^

Begin by creating the source file in ``shell/xsh_echo.c``. Since all
commands take the same arguments (as passed by the shell), we get:

.. code:: c

    /**
     * @file     xsh_echo.c
     * @provides xsh_echo
     *
     * $Id$
     */
    /* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

    #include <kernel.h>
    #include <stdio.h>
    #include <string.h>

    /**
     * Shell command echos input text to standard out.
     * @param stdin descriptor of input device
     * @param stdout descriptor of output device
     * @param stderr descriptor of error device
     * @param args array of arguments
     * @return OK for success, SYSERR for syntax error
     */
    command xsh_echo(ushort nargs, char *args[])
    {
        int i;  /* counter for looping through arguments */

        /* Output help, if '--help' argument was supplied */
        if (nargs == 2 && strncmp(args[1],"--help",6) == 0)
        {
            fprintf(stdout, "Usage: clear\n");
            fprintf(stdout, "Clears the terminal.\n");
            fprintf(stdout, "\t--help\t display this help and exit\n");
            return SYSERR;
        }

        /* loop through the arguments printing each as it is displayed */
        for ( i = 1; i < nargs; i++ )
        {
            fprintf(stdout, "%s ", args[i]);
        }

        /* Just so the next prompt doesn't run on to this line */
        fprintf(stdout, "\n");

        /* there were no errors so, return OK */
        return OK;
    }

Add the function to the command table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While we are in the ``shell/`` directory, we'll modify the command table
found at the top of ``shell/shell.c``. Since we are adding the echo
command, we'll most likely want the user input at the shell to be
"``echo``," this is not a builtin function (FALSE), and the function
that supports this is xsh\_echo. Giving us the entry:

.. code:: c

    { "echo", FALSE, xsh_echo }

Add the function prototype to the include file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Next we must add the prototype of the function to the shell include file
in ``include/shell.h``. This is simply done by adding the line:

.. code:: c

    command xsh_echo(ushort, char *[]);

Add the file to the Makefile
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Finally (and most importantly) we add the function to the Makefile to
make sure that it is built by the compiler. We do this by finding the
line beginning with "``C_FILES =``\ " in ``shell/Makerules`` and adding
xsh\_echo.c to the end of it.

Compile and run, and you should now have a working implementation of the
``echo`` command on your XINU system!
