Max 233A Transceiver
====================

`thumb\|150px\|right\|The MAX233A transceiver chip. <Image:maxim.jpg>`__
Because the power needed to send digital signals through the router is
much less than the standard RS232 power levels, we need a way of ramping
up the voltage enough for signals to be correctly detected by serial
hardware on the other end of the cable we connect. In the same way, we
need to make sure that the ~5V signals coming as input from another
machine do not fry the internal circuitry of the router. The obvious
solution for this problem would be a transformer, but there exists
another, much cooler way: the MAX233A. It requires no extra parts or
special power source, but it correctly changes the voltages so
everything works properly. (See [1] for more information on the
MAX233A.)

References
----------

[1]
http://www.compsys1.com/workbench/On_top_of_the_Bench/Max233_Adapter/max233_adapter.html

See Also
--------

-  `National Semiconductor 16550
   UART <National Semiconductor 16550 UART>`__

