Exception and Interrupt Handling
================================

`Embedded XINU <Embedded XINU>`__ utilizes a interrupt handling system
which allows components to register custom interrupt handlers to the
system at runtime or fall-back to the default trap handler.

MIPS processors will jump to and execute code beginning at
``0x8000 0180`` when an exception or interrupt occurs. Code for handling
traps must occupy no more than 0x20 bytes at this location (eight
instructions), therefore Embedded XINU uses three instructions to jump
to different code which will handle traps more robustly.

In order to handle exceptions (such as TLB misses) efficiently, Embedded
XINU uses an interrupt vector system to quickly read the exception code,
load the registered exception handler, and jump to the handler. If any
of these steps do not exist, the handler will fall-back to the default
trap handler.

If an interrupt has occurred it is important to save the state of the
processor and handle the interrupt gracefully so the system can continue
running. Thus, if the interrupt handler was called by an interrupt and
not an exception, the code will save the state of the processor and
perform a similar lookup for interrupt handlers.
