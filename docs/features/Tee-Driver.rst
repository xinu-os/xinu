Tee driver
==========

The **tee** driver, named after the :wikipedia:`UNIX tee program <tee
(command)>`, is an |EX| device driver located in
:source:`device/tee/`.  Its purpose is to combine multiple input or
output devices into a single pseudo-device.

Using tee devices
-----------------

Tee devices are associated with zero or more sub-devices when they are
opened, for example:

.. code-block:: c

    open(TEE0, 2, SERIAL0, USBKBD0);

The number `2` is the number of arguments following, each of which is
the number of a device to associate with the tee device.

After the above, when data is read from ``TEE0`` using :source:`getc()
<system/getc.c>` or :source:`read() <system/read.c>`, the data may be
provided by *any* of the subdevices.  In the example above, ``TEE0``
was opened with serial port and :doc:`USB keyboard
<USB-Keyboard-Driver>` subdevices, so data from both the serial port
and USB keyboard will appear on the tee device.

.. note::
    The order in which characters from different subdevices appear on
    the tee device is, in general, unpredictable.

Tee devices can also be used to clone output to multiple devices.  For
example, to clone output to both a serial port and a framebuffer, one
can open a tee device:

.. code-block:: c

    open(TEE0, 2, SERIAL0, FRAMEBUF);

Any data written to ``TEE0`` will then be written to both ``SERIAL0``
and ``FRAMEBUF``.

The tee driver supports disabling reads or writes to specific
subdevices, which is useful if you would like to both read and write
from the same tee device but not all subdevices support both input and
output.

Relationship with TTY driver
----------------------------

Although both the tee driver and the :doc:`TTY driver <TTY-Driver>`
provide "pseudo"-devices, they differ in that the tee driver does not
provide the line and input/output cooking that the TTY driver does,
while the TTY driver cannot wrap around more than one device like the
tee driver can.  However, one can wrap a tee device with a TTY device
to get the features of both drivers.  This makes possible features
such as an interactive shell that normally accepts input from a serial
port, but at the same time also accepts input from a :doc:`USB
keyboard <USB-Keyboard-Driver>` when one is attached.
