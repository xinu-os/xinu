Startup
=======

.. raw:: mediawiki

   {{Historical}}

CFE Jettison
------------

CFE begins executing code at 0x80001000. It passes a firmware handler,
firmware entry point, and an entry point seal. The firmware entry point
handler points to an address in the middle of the heap space (confirmed
once to point to 0x803029FC, which may or may not be a deterministic
address).

Cache Flush
-----------

The L1 instruction and data caches are intialized and flushed.
