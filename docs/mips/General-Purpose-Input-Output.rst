General purpose input and output
================================

.. note::

   This page appears to have been written with :doc:`WRT54GL` routers
   in mind and may not be applicable to other platforms.

**General Purpose Input and Output** (**GPIO**) is a simple method of
communication.  Currently the shell implements the **led** and
**gpiostat** commands to use these ports.

Port Assignments
----------------

========   ===================
Pin        Assignment
========   ===================
GPIO 0     WLAN LED
GPIO 1     Power LED
GPIO 2     Cisco LED White
GPIO 3     Cisco LED Orange
GPIO 4     Cisco Button
GPIO 5     *Unknown*
GPIO 6     Reset Button
GPIO 7     DMZ LED
========   ===================

Registers
---------

There are four control and status registers, each which is 2 bytes.

-  Input (0xb8000060) - If a GPIO pin is set for output, its input bit
   is automatically set to 0.
-  Output (0xb8000064) - If a GPIO pin is set for input, its output bit
   is automatically set to 0.
-  Enable (0xb8000068) - Determines if a GPIO pin is used for input or
   output. A bit value of 0 is input and 1 is output.
-  Control (0xb800006c) - Usage is currently unknown.

LEDs
----

Research Notes:

-  An LED is enabled by setting its enable bit to 1.
-  When an LED is enabled, its input bit becomes 0.
-  Once an LED is enabled, if its output bit is 0, the LED is on and if
   its output bit is 1, the LED is off.

Resources
---------

-  `SD Card Reader
   Driver <http://devel.masikh.org/OpenWRT/mmc/wrt54gl-v1.1/mmc.c>`__
-  `Adding an MMC/SD
   Card <http://wiki.openwrt.org/OpenWrtDocs/Customizing/Hardware/MMC>`__

