Why we add serial ports
=======================

.. raw:: mediawiki

   {{Historical}}

A normal user will interact with the WRT54GL using a pretty web
interface, which runs as a stripped down web server that listens to port
80 and provides access to basic configuration opitions.

When we load our own code to be executed, we are starting with a blank
slate--anything we want the hardware to do we have to code ourselves.
This makes the first steps very difficult: we can't exactly check the
web interface to see if we're doing things right. How can we know that
our code is working properly if there is no way of getting feedback?

Enter the serial console. A serial connection is essentially two wires
(transmit, and receive) over which two computers can agree on timing and
transmit bytes to each other. In our case, these bytes will represent
characters, and the characters will be the output and input of a
console. LinkSys actually includes the hardware for two serial ports on
the WRT54GL as headers on the circuit board. OpenWRT (and LinkSys..?)
runs a console on first serial port by default, so that once a serial
connection is properly made, all one has to do is press enter for a
console to appear.

The serial device is a much more basic service than say, communicating
over the network, so it is a likely candidate for our first form of
communication to achieve. We could have also tried for maybe the LED
lights, but it would be incredibly difficult to intepret output and also
impossible to get input using them. So the serial port was the goal.
