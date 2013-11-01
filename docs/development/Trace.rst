Trace
=====

To assist with debugging of XINU modules, a standard trace interface
has been implemented.  Each module should have code of the following
form located in its main header:

.. code:: c

    //#define TRACE_[MODULE]     TTY1

    #ifdef TRACE_[MODULE]
    #include <stdio.h>
    #define [MODULE]_TRACE(...)     { \
                    fprintf(TRACE_[MODULE], "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
                    fprintf(TRACE_[MODULE], __VA_ARGS__); \
                    fprintf(TRACE_[MODULE], "\n"); }
    #else
    #define [MODULE]_TRACE(...)
    #endif

For example, the :doc:`Routing` module declares ``TRACE_RT`` and
``RT_TRACE(...)`` in :source:`include/route.h`.

``TRACE_[MODULE]`` defines the device to which ``[MODULE]_TRACE(...)``
writes.  If undefined, no tracing is performed.  The line defining
``TRACE_[MODULE]`` can be undefined or defined as needed to disable or
enable tracing for a particular module.  However, please note that
changes enabling tracing should not be checked into revision control.
