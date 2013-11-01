Wiring
======

.. raw:: html

   <center>

Image:MAX233A.png

.. raw:: html

   </center>

| 
| ----
| \* The component on the left represents the UART head on the main
board of the router. The even pins (2,6,4,8,10) represent the data that
is sent and received from the first serial port, the odd pins
(1,5,3,7,9) the second serial port. Each port has lines for power,
ground, transmit, receive, and an unconnected line. These UARTs follow
the standard, three-wire "smart modem" connection strategy, and do not
make use of hardware flow control lines.

-  The component in the middle represents the `MAX233A
   transceiver <Chips>`__ needed to convert the voltage used to transmit
   signals in the router (3.3V) to the RS-232 standard signals to send
   over the serial port.

-  The components on the right represent two standard DB9 jacks. The
   first port is wired to be a DCE with female DB9, implying that this
   console port connects to a computer. The second serial port is wired
   as a DTE, with male DB9, implying that this secondary port will
   connect to other kinds of communications equipment (serial-controlled
   sensors, etc.,) or perhaps another router's console.

-  A computer connecting to the first port would require a standard DB9
   serial cable; connecting a computer or other DTE to the second port
   would require a `Null Modem <Null Modem>`__.

Please note that the diagrams do NOT represent actual placement of the
pins on the hardware, which may have been moved to simplify the diagram.
