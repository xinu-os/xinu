Serial Adapter Diagram
======================

RJ45/DB9 adapters
-----------------

| Below are diagrams for RJ45 to DB9 adapters allowing connection
between an `Etherlite serial bay and XINU
backends <HOWTO:Build Backend Pool>`__ in a pool. The first diagram is
for UART 0 (DTE). The second diagram is for the platform-dependent UART
1 (DCE). `thumb\|400px\|left\|UART 0, DTE <Image:DB9M.png>`__
| `thumb\|400px\|left\|UART 1, DCE <Image:NullModem.png>`__
| == Null Modem == The null modem adapter is required to make
connections between available UART 1 ports on two machines. The above
UART 1 diagram functions as a null modem when connecting directly to
other backends, as well as to the EtherLite serial bay.

EtherLite to Baytech
--------------------

| The third diagram represents a connection between the between a
Baytech serial-controlled power strip and the EtherLite terminal annex.
The final two diagrams are the Baytech/Etherlite diagram broken into two
parts, representing the mating of two individual RJ45/DB9 adapters.
`thumb\|500px\|left\|Wiring diagram for Baytech to Etherlite
connection <Image:BaytechWiring.png>`__
| `thumb\|300px\|left\|same as UART 0 diagram above <Image:DB9M.png>`__
| `thumb\|300px\|left\|Baytech RJ45 to DB9F <Image:baytech.png>`__
