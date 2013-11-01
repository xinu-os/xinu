UART Driver
===========

`right\|450px <Image:UartAsyncDriver.png>`__ The UART driver is a
char-oriented driver designed to work with a `National Semiconductor
16550 UART <National Semiconductor 16550 UART>`__. The driver is
responsible for receiving and sending bytes of data asynchronously.

The UART driver is divided into two sections: an upper half and a lower
half. The two halves communicate via semaphores and buffers. The lower
half is interrupt driven and interacts with the physical hardware. The
upper half of the driver interacts with user programs. It does not
interact directly with the hardware nor does it spinlock while waiting
for the hardware to be ready. The upper half waits on semaphores which
are signaled by the lower half to indicate bytes of data or free space
are avaialable in the appropriate buffer.

Physical UART
-------------

The XINU backends have been equipped with serial ports that are
representative of the National Semiconductor 16550 UART. Documentation
on the 16550 UART can be found at
`http://www.national.com/ds.cgi/NS/NS16C552.pdf <http://www.national.com/ds.cgi/NS/NS16C552.pdf>`__.

Initialization
--------------

Intialize defines the starting values for all members of the control
block: statistical counts are zeroed, buffers are defined, and
semaphores are allocated. Also part of the initialization process is
setting values in the control and status registers:

-  Line control is set to 8 bit, no parity, 1 stop.
-  Receiver FIFO full, transmitt buffer empty, and receiver line status
   interrupts are enabled.
-  Hardware FIFOs are enabled.
-  Divisor Latch bits (high and low)

   -  The divisor can be calculated by using the formula:

    :math:`divisor=\frac{baud\_base+\frac{baud\_rate}{2}}{baud\_rate}.`

Where ``baud_rate`` is the speed you wish to connect at (typically
115,200) and

    :math:`baud\_base=\frac{clockrate}{16}`.

The clockrate should be measured in hertz and may not be equivalent to
the clockspeed. (The WRT54GL, for example, has a hard-coded clockrate of
20,000,000 or 20MHz, while the WRT54G has a clockrate of about 25MHz.)

Upper Half (Read and Write)
---------------------------

Read is part of the upper half of the driver that fills a user supplied
buffer with bytes from the input buffer filled by the lower half of the
driver. If the input buffer is empty, read waits for the lower half to
signal on the input semaphore and indicate bytes are avaiable in the
input buffer.

Write is part of the upper half of the driver and places bytes from a
user supplied buffer into the output buffer read by the lower half of
the driver. If there is no free space in the output buffer, write waits
for the lower half to signal on the output semaphore and indicate free
space is available in the output buffer.

Lower Half (Interrupt Handler)
------------------------------

The interrupt handler is the lower half of the driver. The 16550 UART
sends an interrupt (if enabled) when the transmitter FIFO is empty or
the receiver FIFO has reached its available bytes tigger level. Three
different types of interrupts are handled by the lower half:

-  Line or modem status: The interrupt is merely noted in the UART's
   statistical counts.
-  Receiver hardware FIFO trigger level: The driver moves bytes from the
   UART's receive hardware FIFO into the input buffer. Received bytes
   are read from the UART until the Data Ready bit in the Line Status
   Register is no longer set. The input semaphore is signaled to let the
   upper half know bytes of data are in the input buffer.
-  Transmitter hardware FIFO empty: The lower half fills the UART's
   transmit hardware FIFO from the output buffer. The interrupt handler
   fills the transmit hardware FIFO until the FIFO is full or the output
   buffer is empty. The output semaphore is signaled to let the upper
   half know bytes of space are available in the output buffer.

Control
-------

The control functions are used to set, clear, and get the input and
output flags for the UART driver. Non-blocking flags indicate the upper
half read and write functions should perform as much of the requested
read or write length as possible, but should not block to wait for the
lower half to fill or empty the input or output buffers. When the echo
input flag is set, the UART outputs every byte as it is received in
addition to placing the byte in the input buffer.

Loopback
~~~~~~~~

``UART_ENABLE_LOOPBACK`` and ``UART_DISABLE_LOOPBACK`` control functions
enable and disable hardware loopback. Be aware that loopback is
precarious and must be used carefully. It is recommended that you turn
off interrupts prior to enabling loopback and after disabling loopback
to avoid interleaving output while in loopback mode.

Prior to enabling and disabling hardware loopback, the control function
ensures the transmitter is completely empty and has completed all
previous transmission. When in loopback mode the hardware does not throw
interrupts, so the control functions call the UART interrupt handler
explicitly.

See also
--------

-  `TTY Driver <TTY Driver>`__

