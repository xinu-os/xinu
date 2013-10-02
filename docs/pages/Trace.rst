Trace
=====

To assist with debugging of XINU `modules <Modules>`__ a standard trace
interface is currently being implemented across the subsystems. Each
module should have its own trace macro of the following form: #define
[MODULE]\_TRACE(...) { \\ fprintf(TTY1, "%s:%d (%s) ", \_\_FILE\_\_,
\_\_LINE\_\_, \_\_FUNCTION\_\_); \\ fprintf(TTY1, \_\_VA\_ARGS\_\_); \\
fprintf(TTY1, "\\n"); } The trace macro should be defined within the
subsystem's main header file. For example, the `TTY
driver <TTY_Driver>`__ would have the trace macro ``TTY_TRACE`` defined
in ``tty.h``.

To disable tracing for a particular module, the trace macro definition
can be succeeded by: #undef [MODULE]\_TRACE #define [MODULE]\_TRACE This
redefines a particular module's trace macro as nothing, causing the C
preprocessor to replace all calls to the module's trace function with
nothing. This ability to easily remove trace calls from the module's
code will reduce the size of XINU boot image when debugging using trace
is not needed.

The overarching ``XINU_TRACE`` constant (defined in ``kernel.h``)
enables trace for any module with a trace macro defined. To allow all
tracing to be disabled at once, each modules trace function should check
if ``XINU_TRACE`` is defined and only define ``[MODULE]_TRACE`` as the
trace macro if ``XINU_TRACE`` is defined. A full implementation of a
``[MODULE]_TRACE`` macro should be: #ifdef XINU\_TRACE #define
[MODULE]\_TRACE(...) { \\ fprintf(TTY1, "%s:%d (%s) ", \_\_FILE\_\_,
\_\_LINE\_\_, \_\_FUNCTION\_\_); \\ fprintf(TTY1, \_\_VA\_ARGS\_\_); \\
fprintf(TTY1, "\\n"); } #else #define [MODULE]\_TRACE #endif
